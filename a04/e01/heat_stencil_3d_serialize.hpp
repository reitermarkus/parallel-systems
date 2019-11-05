#include <vector>

#include "../../shared/boost.hpp"

using namespace std;

class x_direction {
  vector<vector<vector<float>>>& matrix;
  size_t x;
  size_t height;
  size_t depth;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;

    for (size_t y = 0; y < height; y++) {
      for (size_t z = 0; z < depth; z++) {
        ar & matrix[y][x][z];
      }
    }
  }

  public:
    x_direction(vector<vector<vector<float>>>& _matrix, size_t _x)
      : matrix(_matrix), x(_x), height(_matrix.size()), depth(_matrix[0][0].size()) {}
};

class y_direction {
  vector<vector<vector<float>>>& matrix;
  size_t y;
  size_t width;
  size_t depth;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;

    for (size_t x = 0; x < width; x++) {
      for (size_t z = 0; z < depth; z++) {
        ar & matrix[y][x][z];
      }
    }
  }

  public:
    y_direction(vector<vector<vector<float>>>& _matrix, size_t _y)
      : matrix(_matrix), y(_y), width(_matrix[0].size()), depth(_matrix[0][0].size()) {}
};

class z_direction {
  vector<vector<vector<float>>>& matrix;
  size_t z;
  size_t height;
  size_t width;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;

    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        ar & matrix[y][x][z];
      }
    }
  }

  public:
    z_direction(vector<vector<vector<float>>>& _matrix, size_t _z)
      : matrix(_matrix), z(_z), height(_matrix.size()), width(_matrix[0].size()) {}
};

class collector {
  vector<vector<vector<float>>>& matrix;
  size_t height;
  size_t width;
  size_t depth;
  size_t start_x;
  size_t start_y;
  size_t start_z;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;

    for (size_t y = start_y; y < start_y + height; y++) {
      for (size_t x = start_x; x < start_x + width; x++) {
        for (size_t z = start_z; z < start_z + depth; z++) {
          ar & matrix[y][x][z];
        }
      }
    }
  }

  public:
    collector(
      vector<vector<vector<float>>>& _matrix,
      size_t _height, size_t _width, size_t _depth,
      size_t _start_y, size_t _start_x, size_t _start_z
    ) : matrix(_matrix),
        height(_height), width(_width), depth(_depth),
        start_x(_start_x), start_y(_start_y), start_z(_start_z) {}
};
