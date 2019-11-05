#include "heat_stencil_3d.hpp"
#include "heat_stencil_3d_serialize.hpp"
#include "../../shared/parse_ull.hpp"
#include "../../shared/boost.hpp"
#include "../../shared/vector.hpp"

int main(int argc, char **argv) {
  size_t room_size = 50;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  mpi::environment env(argc, argv);
  mpi::communicator world;

  size_t size = world.size();
  size_t rank = world.rank();

  size_t dim = size;

  vector<mpi::cartesian_dimension> dimensions = {
    mpi::cartesian_dimension(dim),
  };

  mpi::cartesian_topology topology(dimensions);

  size_t max_rank = 1;
  for (auto t: topology) {
    max_rank *= t.size;
  }

  mpi::cartesian_communicator cart_comm(world, topology);

  if (rank >= max_rank) {
    return EXIT_SUCCESS;
  }

  auto source_x = room_size / 4;
  auto source_y = room_size / 4;
  auto source_z = room_size / 4;

  auto chunk_size = (room_size + dim - 1) / dim;

  vector<vector<vector<float>>> buffer_a(chunk_size + 2, vector<vector<float>>(room_size, vector<float>(room_size, 273.0)));
  vector<vector<vector<float>>> buffer_b(chunk_size + 2, vector<vector<float>>(room_size, vector<float>(room_size)));

  int chunk_rank_y = source_y / chunk_size;

  vector<int> coordinates {chunk_rank_y};

  size_t source_rank = cart_comm.rank(coordinates);

  auto chunk_source_y = source_y % chunk_size + 1;

  if (rank == source_rank) {
    buffer_a[chunk_source_y][source_x][source_z] += 60;
  }

  auto rank_coords = cart_comm.coordinates(rank);
  auto rank_y = rank_coords[0];

  auto [up_source, down_dest] = cart_comm.shifted_ranks(0, 1);
  auto [down_source, up_dest] = cart_comm.shifted_ranks(0, -1);

  size_t time_steps = room_size * 500;

  if (rank == 0) {
    cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";
  }

  for (size_t t = 0; t < time_steps; t++) {
    vector<mpi::request> requests;

    // Send first row to top neighbor.
    if (up_dest >= 0) {
      y_direction upper_level(buffer_a, 1);
      requests.push_back(cart_comm.isend(up_dest, 3, upper_level));
    }

    // Send last row to bottom neighbor.
    if (down_dest >= 0) {
      y_direction lower_level(buffer_a, chunk_size);
      requests.push_back(cart_comm.isend(down_dest, 4, lower_level));
    }

    // Receive last row from bottom neighbor.
    if (down_source >= 0) {
      y_direction lower_level(buffer_a, chunk_size + 1);
      cart_comm.recv(down_source, 3, lower_level);
    }

    // Receive top row from top neighbor.
    if (up_source >= 0) {
      y_direction upper_level(buffer_a, 0);
      cart_comm.recv(up_source, 4, upper_level);
    }

    mpi::wait_all(begin(requests), end(requests));

    for (size_t y = 1; y < chunk_size + 1; y++) {
      for (size_t x = 0; x < room_size; x++) {
        for (size_t z = 0; z < room_size; z++) {
          size_t global_y = rank_y * chunk_size + y - 1;

         // The center stays constant (the heat is still on).
          if (rank == source_rank && y == chunk_source_y && x == source_x && z == source_z) {
            buffer_b[y][x][z] = buffer_a[y][x][z];
            continue;
          }

          if (global_y >= room_size) {
            continue;
          }

          calc_temperature(room_size, x, y, z, x, global_y, z, buffer_a, buffer_b);
        }
      }
    }

    swap(buffer_a, buffer_b);
  }

  auto effective_height = [&](size_t r_y) {
    return (r_y + 1) * chunk_size > room_size ? room_size % chunk_size : chunk_size;
  };

  collector send_buffer(buffer_a, effective_height(rank_y), room_size, room_size, 1, 0, 0);

  auto request = cart_comm.isend(0, 7, send_buffer);

  if (rank == 0) {
    vector<vector<vector<float>>> buffer_c(room_size, vector<vector<float>>(room_size, vector<float>(room_size, 273.0)));

    for (size_t r = 0; r < max_rank; r++) {
      auto r_coords = cart_comm.coordinates(r);
      auto r_y = r_coords[0];

      collector receive_buffer(buffer_c, effective_height(r_y), room_size, room_size, r_y * chunk_size, 0, 0);
      cart_comm.recv(r, 7, receive_buffer);
    }

    request.wait();

    print_temperature(buffer_c);

    return verify(buffer_c);
  } else {
    request.wait();
  }

  return EXIT_SUCCESS;
}
