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

  vector<int> coordinates {chunk_rank_y, chunk_rank_x, chunk_rank_z};

  size_t source_rank = cart_comm.rank(coordinates);

  auto chunk_source_y = source_y % chunk_size + 1;
  auto chunk_source_x = source_x % chunk_size + 1;
  auto chunk_source_z = source_z % chunk_size + 1;

  if (rank == source_rank) {
    buffer_a[chunk_source_y][chunk_source_x][chunk_source_z] += 60;
  }

  auto global_coords = cart_comm.coordinates(rank);
  auto rank_y = global_coords[0];
  auto rank_x = global_coords[1];
  auto rank_z = global_coords[2];

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
     if (rank_x % 2) {
      // Send last column to right neighbor.
      if (right_dest >= 0) {
        x_direction right_side(buffer_a, chunk_size);
        cart_comm.send(right_dest, 2, right_side);
      }

      // Receive left column from left neighbor.r
      if (left_source >= 0) {
        x_direction left_side(buffer_a, 0);
        cart_comm.recv(left_source, 2, left_side);
      }

      // Send first column to left neighbor.
      if (left_dest >= 0) {
        x_direction left_side(buffer_a, 1);
        cart_comm.send(left_dest, 1, left_side);
      }

      // Receive last column from right neighbor.
      if (right_source >= 0) {
        x_direction right_side(buffer_a, chunk_size + 1);
        cart_comm.recv(right_source, 1, right_side);
      }
     } else {
      // Receive left column from left neighbor.r
      if (left_source >= 0) {
        x_direction left_side(buffer_a, 0);
        cart_comm.recv(left_source, 2, left_side);
      }

      // Send last column to right neighbor.
      if (right_dest >= 0) {
        x_direction right_side(buffer_a, chunk_size);
        cart_comm.send(right_dest, 2, right_side);
      }

      // Receive last column from right neighbor.
      if (right_source >= 0) {
        x_direction right_side(buffer_a, chunk_size + 1);
        cart_comm.recv(right_source, 1, right_side);
      }

      // Send first column to left neighbor.
      if (left_dest >= 0) {
        x_direction left_side(buffer_a, 1);
        cart_comm.send(left_dest, 1, left_side);
      }
    }

    if (rank % 2) {
      // Send first row to top neighbor.
      if (up_dest >= 0) {
        y_direction upper_level(buffer_a, 1);
        cart_comm.send(up_dest, 3, upper_level);
      }

      // Receive last row from bottom neighbor.
      if (down_source >= 0) {
        y_direction lower_level(buffer_a, chunk_size + 1);
        cart_comm.recv(down_source, 3, lower_level);
      }

      // Send last row to bottom neighbor.
      if (down_dest >= 0) {
        y_direction lower_level(buffer_a, chunk_size);
        cart_comm.send(down_dest, 4, lower_level);
      }

      // Receive top row from top neighbor.
      if (up_source >= 0) {
        y_direction upper_level(buffer_a, 0);
        cart_comm.recv(up_source, 4, upper_level);
      }
    } else {
      // Receive last row from bottom neighbor.
      if (down_source >= 0) {
        y_direction lower_level(buffer_a, chunk_size + 1);
        cart_comm.recv(down_source, 3, lower_level);
      }

      // Send first row to top neighbor.
      if (up_dest >= 0) {
        y_direction upper_level(buffer_a, 1);
        cart_comm.send(up_dest, 3, upper_level);
      }

      // Receive top row from top neighbor.
      if (up_source >= 0) {
        y_direction upper_level(buffer_a, 0);
        cart_comm.recv(up_source, 4, upper_level);
      }

      // Send last row to bottom neighbor.
      if (down_dest >= 0) {
        y_direction lower_level(buffer_a, chunk_size);
        cart_comm.send(down_dest, 4, lower_level);
      }
    }

    if (rank_z % 2) {
      if (front_dest >= 0) {
        z_direction front_side(buffer_a, 1);
        cart_comm.send(front_dest, 5, front_side);
      }

      if (back_source >= 0) {
        z_direction back_side(buffer_a, chunk_size + 1);
        cart_comm.recv(back_source, 5, back_side);
      }

      if (back_dest >= 0) {
        z_direction back_side(buffer_a, chunk_size);
        cart_comm.send(back_dest, 6, back_side);
      }

      if (front_source >= 0) {
        z_direction front_side(buffer_a, 0);
        cart_comm.recv(front_source, 6, front_side);
      }
    } else {
      if (back_source >= 0) {
        z_direction back_side(buffer_a, chunk_size + 1);
        cart_comm.recv(back_source, 5, back_side);
      }

      if (front_dest >= 0) {
        z_direction front_side(buffer_a, 1);
        cart_comm.send(front_dest, 5, front_side);
      }

      if (front_source >= 0) {
        z_direction front_side(buffer_a, 0);
        cart_comm.recv(front_source, 6, front_side);
      }

      if (back_dest >= 0) {
        z_direction back_side(buffer_a, chunk_size);
        cart_comm.send(back_dest, 6, back_side);
      }
    }

    for (size_t y = 1; y < chunk_size + 1; y++) {
      for (size_t x = 1; x < chunk_size + 1; x++) {
        for (size_t z = 1; z < chunk_size + 1; z++) {
          size_t global_x = rank_x * chunk_size + x - 1;
          size_t global_y = rank_y * chunk_size + y - 1;
          size_t global_z = rank_z * chunk_size + z - 1;

          // The center stays constant (the heat is still on).
          if (rank == source_rank && (y == chunk_source_y && x == chunk_source_x && z == chunk_source_z)) {
            buffer_b[y][x][z] = buffer_a[y][x][z];
            continue;
          }

          if (global_y >= room_size || global_x >= room_size || global_z >= room_size) {
            continue;
          }

          calc_temperature(x, y, z, global_x, global_y, global_z, room_size, buffer_a, buffer_b);
        }
      }
    }

    swap(buffer_a, buffer_b);
  }

  auto effective_height = [&](size_t r_y) {
    return (r_y + 1) * chunk_size > room_size ? room_size % chunk_size : chunk_size;
  };

  auto effective_width = [&](size_t r_x) {
    return (r_x + 1) * chunk_size > room_size ? room_size % chunk_size : chunk_size;
  };

  auto effective_depth = [&](size_t r_z) {
    return (r_z + 1) * chunk_size > room_size ? room_size % chunk_size : chunk_size;
  };

  collector send_buffer(buffer_a, effective_height(rank_y), effective_width(rank_x), effective_depth(rank_z), 1, 1, 1);
  auto request = cart_comm.isend(0, 7, send_buffer);

  if (rank == 0) {
    vector<vector<vector<float>>> buffer_c(room_size, vector<vector<float>>(room_size, vector<float>(room_size, 273.0)));

    for (size_t r = 0; r < max_rank; r++) {
      auto r_coords = cart_comm.coordinates(r);
      auto r_y = r_coords[0];
      auto r_x = r_coords[1];
      auto r_z = r_coords[2];

      collector receive_buffer(buffer_c, effective_height(r_y), effective_width(r_x), effective_depth(r_z), r_y * chunk_size, r_x * chunk_size, r_z * chunk_size);
      cart_comm.recv(r, 7, receive_buffer);
    }

    print_temperature(buffer_c);

    return verify(buffer_c);
  }

  request.wait();

  return EXIT_SUCCESS;
}
