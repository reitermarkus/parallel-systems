use std::env;
use rand::Rng;

struct Particle {
  position: (f32, f32),
  velocity: (f32, f32),
  mass: f32,
}

const G: f32 = 1.0;

fn generate_particle() -> Particle {
  let mut rng = rand::thread_rng();

  Particle {
    position: (rng.gen(), rng.gen()),
    velocity: (rng.gen(), rng.gen()),
    mass: rng.gen(),
  }
}

fn distance(p1: (f32, f32), p2: (f32, f32)) -> f32 {
  ((p1.0 - p2.0).powf(2.0) + (p1.1 - p2.1).powf(2.0)).sqrt()
}

fn calculate_force(p1: &Particle, p2: &Particle) -> f32 {
  let radius = distance(p1.position, p2.position) / 2.0;
  G * (p1.mass * p2.mass) / radius.powf(2.0)
}

fn main() {
  let samples = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(2_000);

  let dt = 1.0;

  let time_steps = 1000;

  let mut particles = (0..samples).map(|_| generate_particle()).collect::<Vec<_>>();

  for _ in 0..time_steps {
    let mut forces = vec![0.0; samples];

    for i in 0..samples {
      for j in 0..samples {
        forces[i] += calculate_force(&particles[i], &particles[j]);
      }
    }

    for i in 0..samples {
      let velocity = forces[i] * dt / particles[i].mass;

      particles[i].velocity = (particles[i].velocity.0 + velocity, particles[i].velocity.1 + velocity);
      particles[i].position = (particles[i].position.0 + particles[i].velocity.0 * dt, particles[i].position.1 + particles[i].velocity.1 * dt);
    }
  }
}
