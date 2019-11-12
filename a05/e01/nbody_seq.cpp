#include "../../shared/parse_ull.hpp"

#include <chrono>
#include <random>
#include <thread>
#include <vector>
#include "assert.h"

using namespace std;
using namespace std::chrono_literals;

static const float G = 1.0;

struct Particle {
  pair<float, float> position;
  pair<float, float> velocity;
  float mass;

  Particle() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis1(-1.0, 1.0);
    uniform_real_distribution<> dis2(0.01, 10.0);

    position = make_pair(dis1(gen), dis1(gen));
    velocity = make_pair(dis1(gen), dis1(gen));
    mass = dis2(gen);
  }
};

void visualize(const vector<Particle>& particles) {
  size_t width = 120;
  size_t height = 40;

  vector<vector<size_t>> screen(height, vector<size_t>(width, 0));

  for (auto p: particles) {
    ssize_t x = p.position.first * (width - 1) + width / 2.0;
    ssize_t y = p.position.second * (height - 1) + height / 2.0;

    if (x >= static_cast<ssize_t>(width) || y >= static_cast<ssize_t>(height) || x < 0 || y < 0) {
      continue;
    }

    screen[static_cast<size_t>(y)][static_cast<size_t>(x)] += 1;
  }

  cout << "┌";

  for (size_t x = 0; x < width; x++) {
    cout << "─";
  }

  cout << "┐" << endl;

  for (size_t y = 0; y < height; y++) {
    cout << "│";

    for (size_t x = 0; x < width; x++) {
      switch (screen[y][x]) {
        case 0: cout << " "; break;
        case 1: cout << "·"; break;
        case 2: cout << "✦"; break;
        case 3: cout << "⭑"; break;
        case 4: cout << "✸"; break;
        default: cout << "⬣"; break;
      }
    }

    cout << "│" << endl;
  }

  cout << "└";

  for (size_t x = 0; x < width; x++) {
    cout << "─";
  }

  cout << "┘" << endl;
}

void advance(vector<Particle>& particles, const float dt) {
  for (size_t i = 0; i < particles.size(); i++) {
    for (size_t j = i + 1; j < particles.size(); j++) {
      auto dx = particles[i].position.first - particles[j].position.first;
      auto dy = particles[i].position.second - particles[j].position.second;

      auto radius = sqrt(powf(dx, 2.0) + powf(dy, 2.0));
      assert(radius > 0.0);

      auto force = G * (particles[i].mass * particles[i].mass) / powf(radius, 2.0);

      auto velocity_j = force * dt / particles[i].mass;

      particles[i].velocity = make_pair(
        particles[i].velocity.first - dx * velocity_j,
        particles[i].velocity.second - dy * velocity_j
      );

      auto velocity_i = force * dt / particles[j].mass;

      particles[j].velocity = make_pair(
        particles[j].velocity.first + dx * velocity_i,
        particles[j].velocity.second + dy * velocity_i
      );
    }

    particles[i].position = make_pair(
      particles[i].position.first + particles[i].velocity.first * dt,
      particles[i].position.second + particles[i].velocity.second * dt
    );
  }
}

int main(int argc, char **argv) {
  size_t samples = 1000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  float dt = 0.0001;

  size_t time_steps = 1000;

  vector<Particle> particles(samples);

  for (size_t i = 0; i < particles.size(); i++) {
    particles[i] = Particle();
  }

  visualize(particles);

  for (size_t t = 0; t < time_steps; t++) {
    advance(particles, dt);

    this_thread::sleep_for(50ms);
    cout << "t = " << t << endl;
    visualize(particles);
  }

  return EXIT_SUCCESS;
}
