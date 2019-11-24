#include <random>

#include "../shared/nbody.hpp"

inline void advance(vector<Particle>& particles, const float dt, vector<float>& velocities_x, vector<float>& velocities_y) {
  #pragma omp parallel for num_threads(THREADS) 
  for (size_t i = 0; i < particles.size(); i++) {
    for (size_t j = i + 1; j < particles.size(); j++) {
      auto dx = particles[i].position.first - particles[j].position.first;
      auto dy = particles[i].position.second - particles[j].position.second;

      auto radius = sqrt(powf(dx, 2.0) + powf(dy, 2.0));
      #ifdef DEBUG
        assert(radius > 0.0);
      #endif

      auto force = G / powf(radius, 2.0) * dt;

      auto velocity_j = force * particles[j].mass;
      velocities_x[i] -= dx * velocity_j;
      velocities_y[i] -= dy * velocity_j;

      auto velocity_i = force * particles[i].mass;
      velocities_x[j] += dx * velocity_i;
      velocities_y[j] += dy * velocity_i;
    }
  }


  for (size_t i = 0; i < particles.size(); i++) {
    particles[i].position.first += velocities_x[i] * dt;
    particles[i].position.second += velocities_y[i] * dt;
  }
}

int main(int argc, char **argv) { 
  size_t samples = 1000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  float dt = 0.0001;

  size_t time_steps = 1000;

  #ifdef DEBUG
    mt19937 gen(1);
  #else
    random_device rd;
    mt19937 gen(rd());
  #endif

  vector<Particle> particles;
  particles.reserve(samples);

  for (size_t i = 0; i < samples; i++) {
    particles.emplace_back(gen);
  }

  #ifdef DEBUG
    visualize(particles);
  #endif

  vector<float> velocities_x(particles.size(), 0.0);
  vector<float> velocities_y(particles.size(), 0.0);

  for (size_t t = 0; t < time_steps; t++) {
    advance(particles, dt, velocities_x, velocities_y);

    #ifdef DEBUG
      this_thread::sleep_for(50ms);
      cout << "t = " << t << endl;
      visualize(particles);
    #endif
  }

  return EXIT_SUCCESS;
}
