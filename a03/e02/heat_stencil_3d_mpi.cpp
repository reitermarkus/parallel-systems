#include "heat_stencil_3d.hpp"
#include "../../shared/parse_ull.hpp"
#include "../boost.hpp"

int main(int argc, char **argv) {
  size_t room_size = 500;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  mpi::environment env(argc, argv);
  mpi::communicator world;

  size_t size = world.size();
  size_t rank = world.rank();

  size_t dim = floor(cbrtf(static_cast<float>(size)));

  vector<mpi::cartesian_dimension> dimensions = {
    mpi::cartesian_dimension(dim),
    mpi::cartesian_dimension(dim),
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

  vector<vector<vector<float>>> buffer_a(chunk_size + 2, vector<vector<float>>(chunk_size + 2, vector<float>(chunk_size + 2, 273.0)));
  vector<vector<vector<float>>> buffer_b(chunk_size + 2, vector<vector<float>>(chunk_size + 2, vector<float>(chunk_size + 2)));

  int chunk_rank_x = source_x / chunk_size;
  int chunk_rank_y = source_y / chunk_size;
  int chunk_rank_z = source_z / chunk_size;

  vector<int> coordinates {chunk_rank_x, chunk_rank_y, chunk_rank_z};

  size_t source_rank = cart_comm.rank(coordinates);

  auto chunk_source_y = source_y % chunk_size + 1;
  auto chunk_source_x = source_x % chunk_size + 1;
  auto chunk_source_z = source_z % chunk_size + 1;

  if (rank == source_rank) {
    buffer_a[chunk_source_y][chunk_source_x][chunk_source_z] += 60;
  }

  auto [up_source, down_dest] = cart_comm.shifted_ranks(0, 1);
  auto [down_source, up_dest] = cart_comm.shifted_ranks(0, -1);
  auto [left_source, right_dest] = cart_comm.shifted_ranks(1, 1);
  auto [right_source, left_dest] = cart_comm.shifted_ranks(1, -1);
  auto [front_source, back_dest] = cart_comm.shifted_ranks(2, 1);
  auto [back_source, front_dest] = cart_comm.shifted_ranks(2, -1);

  size_t time_steps = room_size * 500;

  if (rank == 0) {
    cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";
  }

  for (size_t t = 0; t < time_steps; t++) {
    // Send first column to left neighbor.
    if (left_dest >= 0) {
      // column left_column(buffer_a, 1);
      // cart_comm.isend(left_dest, 1, left_column);
    }

    // Receive last column from right neighbor.
    if (right_source >= 0) {
      // column right_column(buffer_a, chunk_size + 1);
      // cart_comm.recv(right_source, 1, right_column);
    }

    // Send last column to right neighbor.
    if (right_dest >= 0) {
      // column right_column(buffer_a, chunk_size);
      // cart_comm.isend(right_dest, 2, right_column);
    }

    // Receive left column from left neighbor.
    if (left_source >= 0) {
      // column left_column(buffer_a, 0);
      // cart_comm.recv(left_source, 2, left_column);
    }

    // Send first row to top neighbor.
    if (up_dest >= 0) {
      // cart_comm.isend(up_dest, 3, buffer_a[1]);
    }

    // Receive last row from bottom neighbor.
    if (down_source >= 0) {
      // cart_comm.recv(down_source, 3, buffer_a[chunk_size + 1]);
    }

    // Send last row to bottom neighbor.
    if (down_dest >= 0) {
      // cart_comm.isend(down_dest, 4, buffer_a[chunk_size]);
    }

    // Receive top row from top neighbor.
    if (up_source >= 0) {
      // cart_comm.recv(up_source, 4, buffer_a[0]);
    }
  }
}
