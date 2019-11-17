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

  size_t dim = size;

  vector<mpi::cartesian_dimension> dimensions = { mpi::cartesian_dimension(dim) };
  mpi::cartesian_topology topology(dimensions);
  mpi::cartesian_communicator cart_comm(world, topology);

  size_t chunk_size = (samples + dim - 1) / dim;

  auto [left, right] = cart_comm.shifted_ranks(0, 1);

  vector<Particle> particles(samples);

  for (size_t i = 0; i < particles.size(); i++) {
    particles[i] = Particle();
  }

  visualize(particles);

  size_t max_i = clamp<size_t>(rank * chunk_size + chunk_size, 0, samples);

  for (size_t t = 0; t < time_steps; t++) {
    vector<float> velocity_offset_x(samples, 0.0);
    vector<float> velocity_offset_y(samples, 0.0);

    for (size_t i = rank * chunk_size; i < particles.size(); i++) {
      for (size_t j = i + 1; j < particles.size(); j++) {
        auto dx = particles[i].position.first - particles[j].position.first;
        auto dy = particles[i].position.second - particles[j].position.second;

        auto radius = sqrt(powf(dx, 2.0) + powf(dy, 2.0));

        #define DEBUG
        #ifdef DEBUG
          assert(radius > 0.0);
        #endif

        auto force = G / powf(radius, 2.0) * dt;

        auto velocity_j = force * particles[j].mass;
        velocity_offset_x[i] -= dx * velocity_j;
        velocity_offset_y[i] -= dy * velocity_j;

        auto velocity_i = force * particles[i].mass;
        velocity_offset_x[i] += dx * velocity_i;
        velocity_offset_y[i] += dy * velocity_i;
      }
    }

    vector<float> received_velocity_offset_x(samples);
    mpi::all_reduce(cart_comm, &velocity_offset_x[0], samples, &received_velocity_offset_x[0], plus<float>());
    vector<float> received_velocity_offset_y(samples);
    mpi::all_reduce(cart_comm, &velocity_offset_y[0], samples, &received_velocity_offset_y[0], plus<float>());

    for (size_t i = 0; i < particles.size(); i++) {
      particles[i].velocity = make_pair(
        particles[i].velocity.first + received_velocity_offset_x[i],
        particles[i].velocity.second + received_velocity_offset_y[i]
      );

      particles[i].position = make_pair(
        particles[i].position.first + particles[i].velocity.first * dt,
        particles[i].position.second + particles[i].velocity.second * dt
      );
    }

    if (rank == 0) {
      this_thread::sleep_for(50ms);
      cout << "t = " << t << endl;
      visualize(particles);
    }
  }

  return EXIT_SUCCESS;
}
