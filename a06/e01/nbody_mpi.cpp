#include <random>

#include "../../shared/boost.hpp"
#include "../../shared/nbody.hpp"

int main(int argc, char **argv) {
  size_t samples = 1000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  float dt = 0.0001;

  size_t time_steps = 1000;

  mpi::environment env(argc, argv);
  mpi::communicator world;

  size_t size = world.size();
  size_t rank = world.rank();

  size_t chunk_size = (samples + size - 1) / size;

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
    if (rank == 0) {
      visualize(particles);
    }
  #endif

  size_t max_i = clamp<size_t>(rank * chunk_size + chunk_size, 0, particles.size());

  vector<float> local_velocity_offset_x(samples, 0.0);
  vector<float> local_velocity_offset_y(samples, 0.0);

  vector<float> velocity_offset_x(samples);
  vector<float> velocity_offset_y(samples);

  for (size_t t = 0; t < time_steps; t++) {
    for (size_t i = rank * chunk_size; i < max_i; i++) {
      for (size_t j = i + 1; j < particles.size(); j++) {
        auto dx = particles[i].position.first - particles[j].position.first;
        auto dy = particles[i].position.second - particles[j].position.second;

        auto radius = sqrt(powf(dx, 2.0) + powf(dy, 2.0));
        #ifdef DEBUG
          assert(radius > 0.0);
        #endif

        auto force = G / powf(radius, 2.0) * dt;

        auto velocity_j = force * particles[j].mass;
        local_velocity_offset_x[i] -= dx * velocity_j;
        local_velocity_offset_y[i] -= dy * velocity_j;

        auto velocity_i = force * particles[i].mass;
        local_velocity_offset_x[j] += dx * velocity_i;
        local_velocity_offset_y[j] += dy * velocity_i;
      }
    }

    mpi::all_reduce(world, &local_velocity_offset_x[0], samples, &velocity_offset_x[0], plus<float>());
    mpi::all_reduce(world, &local_velocity_offset_y[0], samples, &velocity_offset_y[0], plus<float>());

    for (size_t i = 0; i < particles.size(); i++) {
      particles[i].position.first += velocity_offset_x[i] * dt;
      particles[i].position.second += velocity_offset_y[i] * dt;
    }

    #ifdef DEBUG
      if (rank == 0) {
        this_thread::sleep_for(50ms);
        cout << "t = " << t << endl;
        visualize(particles);
      }
    #endif
  }

  return EXIT_SUCCESS;
}
