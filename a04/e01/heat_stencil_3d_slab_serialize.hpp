#include <vector>

#include "../../shared/boost.hpp"

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
  size_t deserialize;
  size_t start_x;
  size_t start_y;
  size_t start_z;
  size_t height;
  size_t width;
  size_t depth;
  size_t room_size;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    (void)version;

    ar & start_x;
    ar & start_y;
    ar & start_z;

    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        for (size_t z = 0; z < depth; z++) {
          if (start_y * height + y >= room_size || start_x * height + x >= room_size) {
            continue;
          }

          if (deserialize) {
            ar & matrix[start_y * height + y][start_x * width + x][z];
          } else {
            ar & matrix[y + 1][x][z];
          }
        }
      }
    }
  }

  public:
    collector(vector<vector<vector<float>>>& _matrix, size_t _height, size_t _width, size_t _depth, size_t _x, size_t _y, size_t _z, bool _deserialize, size_t _room_size)
      : matrix(_matrix), deserialize(_deserialize), start_x(_x), start_y(_y), start_z(_z), height(_height), width(_width), depth(_depth), room_size(_room_size) {}
};
