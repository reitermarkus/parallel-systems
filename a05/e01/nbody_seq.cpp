#include "../../shared/nbody.hpp"

inline void advance(vector<Particle>& particles, const float dt) {
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
      particles[i].velocity.first -= dx * velocity_j;
      particles[i].velocity.second -= dy * velocity_j;

      auto velocity_i = force * particles[i].mass;
      particles[j].velocity.first += dx * velocity_i;
      particles[j].velocity.second += dy * velocity_i;
    }

    particles[i].position.first += particles[i].velocity.first * dt;
    particles[i].position.second += particles[i].velocity.second * dt;
  }
}

int main(int argc, char **argv) {
  size_t samples = 1000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  float dt = 0.0001;

  size_t time_steps = 1000;

  vector<Particle> particles(samples);

  for (size_t i = 0; i < particles.size(); i++) {
    particles[i] = Particle();
  }

  visualize(particles);

  for (size_t t = 0; t < time_steps; t++) {
    advance(particles, dt);

    this_thread::sleep_for(50ms);
    cout << "t = " << t << endl;
    visualize(particles);
  }

  return EXIT_SUCCESS;
}
