#[deny(missing_debug_implementations)]

use std::{env, thread::sleep, time::Duration};
use rand::Rng;

#[derive(Debug)]
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
    velocity: (rng.gen_range(-1.0, 1.0), rng.gen_range(-1.0, 1.0)),
    mass: rng.gen_range(0.01, 10.0),
  }
}

fn calculate_force(p1: &Particle, p2: &Particle) -> f32 {
  let radius = p1.distance_to(p2);
  assert!(radius > 0.0);
  G * (p1.mass * p2.mass) / radius.powf(2.0)
}

fn visualize(particles: &[Particle]) {
  let width = 120;
  let height = 40;

  let mut screen = vec![vec![' '; width]; height];

  for p in particles {
    let mut x = (p.position.0 * (width - 1) as f32 + width as f32 / 2.0) as isize;
    let mut y = (p.position.1 * (height - 1) as f32 + height as f32 / 2.0) as isize;

    if x >= width as isize || y >= height as isize || x < 0 || y < 0 {
      continue;
    }

    screen[y as usize][x as usize] = '*';
  }

  print!("┌");

  for x in 0..width {
    print!("─");
  }

  println!("┐");

  for y in 0..height {
    print!("│");


    for x in 0..width {
      print!("{}", screen[y][x]);
    }

    println!("│");
  }

  print!("└");

  for x in 0..width {
    print!("─");
  }

  println!("┘");
}

fn main() {
  let samples = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(1_000);

  let dt = 0.001;

  let time_steps = 25;

  let mut particles = (0..samples).map(|_| generate_particle()).collect::<Vec<_>>();

  visualize(&particles);

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

    sleep(Duration::from_millis(250));
    println!("t = {}", t);
    visualize(&particles);
  }
}
