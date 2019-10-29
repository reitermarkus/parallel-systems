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

int main(int argc, char **argv) {
  auto room_size = 500;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  mpi::environment env(argc, argv);
  mpi::communicator world;

  int size = world.size();
  int rank = world.rank();

  unsigned long long dim = floor(sqrt(static_cast<float>(size)));

  vector<mpi::cartesian_dimension> dimensions = {
    mpi::cartesian_dimension(dim),
    mpi::cartesian_dimension(dim),
  };

  mpi::cartesian_topology topology(dimensions);

  auto max_rank = reduce(topology.begin(), topology.end(), 1, [&] (int acc, mpi::cartesian_dimension elem) {
    return acc * elem.size;
  });

  mpi::cartesian_communicator cart_comm(world, topology);

  if (rank >= max_rank) {
    return EXIT_SUCCESS;
  }

  auto source_x = room_size - 1;
  auto source_y = room_size - 1;


  // 0 1 2
  // 3 4 5 
  // 6 7 8

  size_t width = room_size / dim + 2;
  size_t height = room_size / dim + 2;

  vector<vector<float>> buffer_a(height, vector<float>(width, 273));
  vector<vector<float>> buffer_b(height, vector<float>(width));

  auto chunk_size = (room_size / dim);

  size_t source_rank = (source_y / chunk_size) * dim + (source_x / chunk_size);

  if (rank == source_rank) {
    buffer_a[source_y % chunk_size + 1][source_x % chunk_size + 1] += 60;
  }

  auto [up_source, down_dest] = cart_comm.shifted_ranks(0, 1);
  auto [down_source, up_dest] = cart_comm.shifted_ranks(0, -1);
  auto [left_source, right_dest] = cart_comm.shifted_ranks(1, 1);
  auto [right_source, left_dest] = cart_comm.shifted_ranks(1, -1);

  auto time_steps = room_size * 100;

  if (rank == 0) {
    cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";
  }

  for (auto t = 0; t < time_steps; t++) {
    // Send first column to left neighbor.
    if (left_dest >= 0) {
      column left_column(buffer_a, 1);
      cart_comm.isend(left_dest, 1, left_column);
    }

    // Receive last column from right neighbor.
    if (right_source >= 0) {
      column right_column(buffer_a, width - 1);
      cart_comm.recv(right_source, 1, right_column);
    }

    // Send last column to right neighbor.
    if (right_dest >= 0) {
      column right_column(buffer_a, width - 2);
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
      cart_comm.recv(down_source, 3, buffer_a[height - 1]);
    }

    // Send last row to bottom neighbor.
    if (down_dest >= 0) {
      cart_comm.isend(down_dest, 4, buffer_a[height - 2]);
    }

    // Receive top row from top neighbor.
    if (up_source >= 0) {
      cart_comm.recv(up_source, 4, buffer_a[0]);
    }

    for (auto i = 1; i < chunk_size + 1; i++) {
      for (auto j = 1; j < chunk_size + 1; j++) {
        if (rank == source_rank && (i == source_y % chunk_size && j == source_x % chunk_size)) {
          // The center stays constant (the heat is still on).
          buffer_b[i][j] = buffer_a[i][j];
          continue;
        }

        // Get temperature at current position.
        float temp_current = buffer_a[i][j];

        // Get temperatures of adjacent cells.
        float temp_left = (j != 0) ? buffer_a[i][j - 1] : temp_current;
        float temp_right = (j != room_size - 1) ? buffer_a[i][j + 1] : temp_current;
        float temp_up = (i != 0) ? buffer_a[i - 1][j] : temp_current;
        float temp_down = (i != room_size - 1) ? buffer_a[i + 1][j] : temp_current;

        // Compute new temperature at current position.
        buffer_b[i][j] = temp_current + 0.2 * (temp_left + temp_right + temp_up + temp_down + (-4 * temp_current));
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);
  }

  vector<float> buffer_a_flat(begin(buffer_a[0]), end(buffer_a[0]));

  for (auto i = 1; i < buffer_a.size(); i++) {
    buffer_a_flat.insert(end(buffer_a_flat), begin(buffer_a[i]), end(buffer_a[i]));
  }

  if (rank == 0) {
    vector<vector<float>> buffer_c(room_size, vector<float>(room_size));

    for (auto r = 0; r < max_rank; r++) {
      if (r > 0) {
        world.recv(r, 5, buffer_a_flat);

        for (size_t i = 0; i < height; i++) {
          buffer_a[i] = vector<float>(&buffer_a_flat[i * width], &buffer_a_flat[i * width + width - 1]);
        }

        cout << "Received buffer from rank " << r << endl;
      }
      
      for (size_t i = 1; i < chunk_size; i++) {
        for (size_t j = 1; j < chunk_size; j++) {
          buffer_c[r / dim + (i - 1)][r % dim + (j - 1)] = buffer_a[i][j];
        }
      }
    }

    print_temperature(buffer_c, room_size, room_size);
  } else {
  /*
  vector<float> buffer_a_flat(buffer_a.size() * buffer_a[0].size());

    for (auto i = 0; i < height; i++) {
      for (auto j = 0; j < width; j++) {
        buffer_a_flat[i * width + j] = buffer_a[i][j];
      }
    }
*/
    world.send(0, 5, buffer_a_flat);
  }


  return 0;

  /*

  cout << " final" << endl;
  print_temperature(buffer_a, room_size, room_size);

  for (auto i = 0; i < room_size; i++) {
    for (auto j = 0; j < room_size; j++) {
      auto temp = buffer_a[i][j];
      if (temp < 273.0 || temp > 273.0 + 60.0) {
        cout << "failed at i: " << i << " " << temp << endl;
        cout << "failed at j: " << j << " " << temp << endl;
        cout << "Verification: FAILED" << endl;
        return EXIT_FAILURE;
      }
    }
  }

  cout << "Verification: OK" << endl;

  return EXIT_SUCCESS;

  */
}
