#include "../../shared/heat_stencil.hpp"
#include "../boost.hpp"

class column {
  vector<vector<float>>& matrix;
  size_t n_rows;
  size_t col;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & n_rows;

    for (size_t i = 0; i < n_rows; i++) {
      ar & matrix[i][col];
    }
  }

  public:
    column(vector<vector<float>>& _matrix, size_t _col)
      : matrix(_matrix), n_rows(_matrix.size()), col(_col) {}
};

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  out << '[';
  if (!v.empty()) {
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
  }
  out << "\b\b]";
  return out;
}

int main(int argc, char **argv) {
  size_t room_size = 500;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  mpi::environment env(argc, argv);
  mpi::communicator world;

  size_t size = world.size();
  size_t rank = world.rank();

  size_t dim = floor(sqrt(static_cast<float>(size)));

  vector<mpi::cartesian_dimension> dimensions = {
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

  auto chunk_size = (room_size + dim - 1) / dim;

  auto global_column = [&](size_t rank, size_t j) {
    return (rank % dim) * chunk_size + j - 1;
  };

  auto global_row = [&](size_t rank, size_t i) {
    return (rank / dim) * chunk_size + i - 1;
  };

  vector<vector<float>> buffer_a(chunk_size + 2, vector<float>(chunk_size + 2, 273));
  vector<vector<float>> buffer_b(chunk_size + 2, vector<float>(chunk_size + 2));

  size_t source_rank = (source_y / chunk_size) * dim + (source_x / chunk_size);

  auto chunk_source_y = source_y % chunk_size + 1;
  auto chunk_source_x = source_x % chunk_size + 1;

  if (rank == source_rank) {
    buffer_a[chunk_source_y][chunk_source_x] += 60;
  }

  auto [up_source, down_dest] = cart_comm.shifted_ranks(0, 1);
  auto [down_source, up_dest] = cart_comm.shifted_ranks(0, -1);
  auto [left_source, right_dest] = cart_comm.shifted_ranks(1, 1);
  auto [right_source, left_dest] = cart_comm.shifted_ranks(1, -1);

  size_t time_steps = room_size * 500;

  if (rank == 0) {
    cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";
  }

  for (size_t t = 0; t < time_steps; t++) {
    // Send first column to left neighbor.
    if (left_dest >= 0) {
      column left_column(buffer_a, 1);
      cart_comm.isend(left_dest, 1, left_column);
    }

    // Receive last column from right neighbor.
    if (right_source >= 0) {
      column right_column(buffer_a, chunk_size + 1);
      cart_comm.recv(right_source, 1, right_column);
    }

    // Send last column to right neighbor.
    if (right_dest >= 0) {
      column right_column(buffer_a, chunk_size);
      cart_comm.isend(right_dest, 2, right_column);
    }

    // Receive left column from left neighbor.
    if (left_source >= 0) {
      column left_column(buffer_a, 0);
      cart_comm.recv(left_source, 2, left_column);
    }

    // Send first row to top neighbor.
    if (up_dest >= 0) {
      cart_comm.isend(up_dest, 3, buffer_a[1]);
    }

    // Receive last row from bottom neighbor.
    if (down_source >= 0) {
      cart_comm.recv(down_source, 3, buffer_a[chunk_size + 1]);
    }

    // Send last row to bottom neighbor.
    if (down_dest >= 0) {
      cart_comm.isend(down_dest, 4, buffer_a[chunk_size]);
    }

    // Receive top row from top neighbor.
    if (up_source >= 0) {
      cart_comm.recv(up_source, 4, buffer_a[0]);
    }

    for (size_t i = 1; i < chunk_size + 1; i++) {
      for (size_t j = 1; j < chunk_size + 1; j++) {
        // The center stays constant (the heat is still on).
        if (rank == source_rank && (i == chunk_source_y && j == chunk_source_x)) {
          buffer_b[i][j] = buffer_a[i][j];
          continue;
        }

        auto gi = global_row(rank, i);
        auto gj = global_column(rank, j);

        if (gi >= room_size || gj >= room_size) {
          continue;
        }

        // Get temperature at current position.
        float temp_current = buffer_a[i][j];

        bool first_column = rank % dim == 0 && j == 1;
        bool last_column = gj == room_size - 1;
        bool first_row = rank < dim && i == 1;
        bool last_row = gi == room_size - 1;

        // Get temperatures of adjacent cells.
        float temp_left = first_column ? temp_current : buffer_a[i][j - 1];
        float temp_right = last_column ? temp_current : buffer_a[i][j + 1];
        float temp_up = first_row ? temp_current : buffer_a[i - 1][j];
        float temp_down = last_row ? temp_current : buffer_a[i + 1][j];

        // Compute new temperature at current position.
        buffer_b[i][j] = temp_current + 0.2 * (temp_left + temp_right + temp_up + temp_down + (-4 * temp_current));
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);
  }

  vector<float> buffer_a_flat(begin(buffer_a[0]), end(buffer_a[0]));

  for (size_t i = 1; i < buffer_a.size(); i++) {
    buffer_a_flat.insert(end(buffer_a_flat), begin(buffer_a[i]), end(buffer_a[i]));
  }

  if (rank == 0) {
    vector<vector<float>> buffer_c(room_size, vector<float>(room_size, 0));

    for (size_t r = 0; r < max_rank; r++) {
      if (r > 0) {
        world.recv(r, 5, buffer_a_flat);

        for (size_t i = 0; i < chunk_size + 2; i++) {
          buffer_a[i] = vector<float>(&buffer_a_flat[i * (chunk_size + 2)], &buffer_a_flat[(i + 1) * (chunk_size + 2)]);
        }
      }

      for (size_t i = 1; i < chunk_size + 2; i++) {
        for (size_t j = 1; j < chunk_size + 2; j++) {
          auto gi = global_row(r, i);
          auto gj = global_column(r, j);

          if (gi >= room_size || gj >= room_size) {
            continue;
          }

          buffer_c[gi][gj] = buffer_a[i][j];
        }
      }
    }

    print_temperature(buffer_c, room_size, room_size);

    for (size_t i = 0; i < room_size; i++) {
      for (size_t j = 0; i < room_size; i++) {
        auto temp = buffer_c[i][j];
        if (temp < 273.0 || temp > 273.0 + 60.0) {
          cout << "wrong temperature at " << i << ", " << j << ": " << temp << endl;
          cout << "Verification: FAILED" << endl;
          return EXIT_FAILURE;
        }
      }
    }

    cout << "Verification: OK" << endl;
  } else {
    world.send(0, 5, buffer_a_flat);
  }

  return EXIT_SUCCESS;
}
