use std::env;
use rand::Rng;

struct Particle {
  position: (f32, f32),
  velocity: (f32, f32),
  mass: f32,
}

impl Particle {
  pub fn distance_to(&self, other: &Self) -> f32 {
    let dx = self.position.0 - other.position.0;
    let dy = self.position.1 - other.position.1;

    (dx.powf(2.0) + dy.powf(2.0)).sqrt()
  }
}

const G: f32 = 1.0;

fn generate_particle() -> Particle {
  let mut rng = rand::thread_rng();

  Particle {
    position: (rng.gen_range(-1.0, 1.0), rng.gen_range(-1.0, 1.0)),
    velocity: (rng.gen_range(-10.0, 10.0), rng.gen_range(-10.0, 10.0)),
    mass: rng.gen_range(1.0, 100.0),
  }
}

fn calculate_force(p1: &Particle, p2: &Particle) -> f32 {
  let radius = p1.distance_to(p2) / 2.0;

  if radius == 0.0 {
    return 0.0
  }

  G * (p1.mass * p2.mass) / radius.powf(2.0)
}

fn visualize(particles: &[Particle]) {
  let width = 120;
  let height = 40;

  let mut screen = vec![vec![' '; width]; height];

  let mut max_y = 0.0;
  let mut max_x = 0.0;

  for p in particles {
    max_x = if p.position.0 > max_x { p.position.0 } else { max_x };
    max_y = if p.position.1 > max_y { p.position.1 } else { max_y };
  }

  for p in particles {
    let mut x = p.position.0 / max_x * (width - 1) as f32;
    let mut y = p.position.1 / max_y * (height - 1) as f32;

    x += width as f32 / 2.0;
    y += height as f32 / 2.0;

    if x > width as f32 || y > height as f32 {
      continue;
    }

    screen[y as usize][x as usize] = '*';
  }

  for y in 0..height {
    for x in 0..width {
      print!("{}", screen[y][x]);
    }

    println!();
  }
}

fn main() {
  let samples = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(1_000);

  let dt = 100.0;

  let time_steps = 10000;

  let mut particles = (0..samples).map(|_| generate_particle()).collect::<Vec<_>>();

  for t in 0..time_steps {
    let mut forces = vec![0.0; samples];

    for i in 0..samples {
      for j in 0..samples {
        if i == j { continue }

        forces[i] += calculate_force(&particles[i], &particles[j]);
      }
    }

    for i in 0..samples {
      let velocity = forces[i] * dt / particles[i].mass;

      particles[i].velocity = (particles[i].velocity.0 + velocity, particles[i].velocity.1 + velocity);
      particles[i].position = (particles[i].position.0 + particles[i].velocity.0 * dt, particles[i].position.1 + particles[i].velocity.1 * dt);
    }

    if t % 1000 == 0 {
      println!("t = {}", t);
      visualize(&particles);
    }
  }
}
