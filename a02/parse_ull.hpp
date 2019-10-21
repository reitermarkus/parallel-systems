#pragma once

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

unsigned long long parse_ull(char* s) {
  errno = 0;

  auto n = strtoull(s, nullptr, 10);

  if (errno != 0) {
    cerr << "Failed parsing '" << s << "' to number: " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }

  return n;
}
