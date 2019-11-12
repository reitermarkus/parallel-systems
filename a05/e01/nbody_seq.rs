#[deny(missing_debug_implementations)]

use std::{env, thread::sleep, time::Duration};
use rand::Rng;

const G: f32 = 1.0;

#[derive(Debug)]
struct Particle {
  position: (f32, f32),
  velocity: (f32, f32),
  mass: f32,
}

impl Particle {
  pub fn generate() -> Self {
    let mut rng = rand::thread_rng();

    Particle {
      position: (rng.gen_range(-1.0, 1.0), rng.gen_range(-1.0, 1.0)),
      velocity: (rng.gen_range(-1.0, 1.0), rng.gen_range(-1.0, 1.0)),
      mass: rng.gen_range(0.01, 10.0),
    }
  }
}

fn visualize(particles: &[Particle]) {
  let width = 120;
  let height = 40;

  let mut screen = vec![vec![0; width]; height];

  for p in particles {
    let x = (p.position.0 * (width - 1) as f32 + width as f32 / 2.0) as isize;
    let y = (p.position.1 * (height - 1) as f32 + height as f32 / 2.0) as isize;

    if x >= width as isize || y >= height as isize || x < 0 || y < 0 {
      continue;
    }

    screen[y as usize][x as usize] += 1;
  }

  print!("┌");

  for _ in 0..width {
    print!("─");
  }

  println!("┐");

  for y in 0..height {
    print!("│");

    for x in 0..width {
      match screen[y][x] {
        0 => print!(" "),
        1 => print!("·"),
        2 => print!("✦"),
        3 => print!("⭑"),
        4 => print!("✸"),
        _ => print!("⬣"),
      }
    }

    println!("│");
  }

  print!("└");

  for _ in 0..width {
    print!("─");
  }

  println!("┘");
}

#[inline(always)]
fn advance(particles: &mut [Particle], dt: f32) {
  for i in 0..particles.len() {
    for j in (i + 1)..particles.len() {
      let dx = particles[i].position.0 - particles[j].position.0;
      let dy = particles[i].position.1 - particles[j].position.1;

      let radius = (dx.powf(2.0) + dy.powf(2.0)).sqrt();
      debug_assert!(radius > 0.0);
      let force = G * (particles[i].mass * particles[i].mass) / radius.powf(2.0);

      let velocity_i = force * dt / particles[j].mass;
      let velocity_j = force * dt / particles[i].mass;

      particles[i].velocity = (
        particles[i].velocity.0 - dx * velocity_j,
        particles[i].velocity.1 - dy * velocity_j,
      );
      particles[j].velocity = (
        particles[j].velocity.0 + dx * velocity_i,
        particles[j].velocity.1 + dy * velocity_i,
      );
    }

    particles[i].position = (
      particles[i].position.0 + particles[i].velocity.0 * dt,
      particles[i].position.1 + particles[i].velocity.1 * dt,
    );
  }
}

fn main() {
  let samples = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(1_000);

  let dt = 0.0001;

  let time_steps = 1_000;

  let mut particles = (0..samples).map(|_| Particle::generate()).collect::<Vec<_>>();

  visualize(&particles);

  for t in 0..time_steps {
    advance(&mut particles, dt);

    sleep(Duration::from_millis(50));
    println!("t = {}", t);
    visualize(&particles);
  }
}
