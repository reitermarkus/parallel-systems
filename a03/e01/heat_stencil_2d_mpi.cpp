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
    ar & col;

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

  vector<vector<float>> buffer_a(room_size / dim + 2, vector<float>(room_size / dim + 2, 273));
  vector<vector<float>> buffer_b(room_size / dim + 2, vector<float>(room_size / dim + 2));

  auto [up_source, down_dest] = cart_comm.shifted_ranks(0, 1);
  auto [down_source, up_dest] = cart_comm.shifted_ranks(0, -1);
  auto [left_source, right_dest] = cart_comm.shifted_ranks(1, 1);
  auto [right_source, left_dest] = cart_comm.shifted_ranks(1, -1);

  if (left_dest >= 0) {
    column left_column(buffer_a, 1);
    cout << "Rank " << rank << " sending to " << left_dest << endl;
    cart_comm.isend(left_dest, 1, left_column);
  }

  if (right_source >= 0) {
    column right_column(buffer_a, dim);
    cart_comm.recv(right_source, 1, right_column);
  }

  if (right_dest >= 0) {
    column right_column(buffer_a, dim);
    cout << "Rank " << rank << " sending to " << right_dest << endl;
    cart_comm.isend(right_dest, 2, right_column);
  }

  if (left_source >= 0) {
    column left_column(buffer_a, 0);
    cart_comm.recv(left_source, 2, left_column);
  }

  if (up_dest >= 0) {
    cout << "Rank " << rank << " sending to " << up_dest << endl;
    cart_comm.isend(up_dest, 3, buffer_a[1]);
  }

  if (down_source >= 0) {
    vector<double> bottom_row(room_size / dim);
    cart_comm.recv(up_source, 3, bottom_row);
  }

  cout << "test " << rank << endl;

  if (down_dest >= 0) {
    cout << "Rank " << rank << " sending to " << down_dest << endl;
    cart_comm.isend(down_dest, 4, buffer_a[dim]);
  }

  if (up_source >= 0) {
    vector<double> top_row(room_size / dim);
    cart_comm.recv(up_source, 4, top_row);
  }

  printf("Rank %d, source %d, target %d", rank, down_source, down_dest);

  return 0;

  /*

  auto time_steps = room_size * 100;

  cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<vector<float>> buffer_a(room_size, vector<float>(room_size, 273.0));
  vector<vector<float>> buffer_b(room_size, vector<float>(room_size));

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  auto source_y = room_size / 4;

  buffer_a[source_y][source_x] = 273.0 + 60.0;

  cout << " initial" << endl;
  print_temperature(buffer_a, room_size, room_size);

  // Propagate the temperature in each time step.
  for (int t = 0; t < time_steps; t++) {
    for (long long i = 0; i < room_size; i++) {
      for (long long j = 0; j < room_size; j++) {
        // The center stays constant (the heat is still on).
        if (i == source_y && j == source_x) {
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
        buffer_b[i][j] = temp_current + 0.1 * (temp_left + temp_right + temp_up + temp_down + (-4 * temp_current));
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);

    // Show intermediate step.
    if (!(t % 1000) && !getenv("CI"))  {
      cout << " t=" << t << endl;
      print_temperature(buffer_a, room_size, room_size);
    }
  }

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
