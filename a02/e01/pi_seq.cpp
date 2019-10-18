#include <cerrno>
#include <cstring>
#include <iostream>
#include <random>

using namespace std;

int main(int argc, char **argv) {
  auto samples = 1000000000;

  if (argc > 1) {
    errno = 0;

    samples = strtol(argv[1], nullptr, 10);

    if (errno != 0) {
      cerr << "Failed parsing '" << argv[1] << "' to number: " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
  }

  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dis(-1, 1);

  unsigned long long inside = 0;
  double pi = 0;

  for (auto i = 1; i <= samples; i++) {
    auto x = dis(gen);
    auto y = dis(gen);

    if (x * x + y * y <= 1.0) {
      inside++;
    }

    if (i % 1000000 == 0 || i == samples) {
      pi = (double)inside / (double)i * 4.0;
      cout << "\rπ = " << pi << flush;
    }
  }

  cout << endl;

  return EXIT_SUCCESS;
}

