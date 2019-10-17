use std::{env, time::Instant};

use mpi::{point_to_point::{Destination, Source}, topology::Communicator};
use rand::Rng;

fn main() {
  let samples = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(1_000_000_000);

  let universe = mpi::initialize().expect("MPI failed to initialize");
  let world = universe.world();
  let size = world.size();
  let rank = world.rank();

  let chunk_size = (samples + size as u64 - 1) / size as u64;
  let start = chunk_size * rank as u64;
  let stop = num::clamp(start + chunk_size, 0, samples);

  if rank == 0 {
    println!("┌──────┬──────────────────────┬──────────────────────┬──────────────────────┐");
    println!("❘ Rank ❘                Start ❘                 Stop ❘                 Time ❘");
    println!("├──────┼──────────────────────┼──────────────────────┼──────────────────────┤");
  }

  let start_time = Instant::now();

  let mut rng = rand::thread_rng();

  let mut inside: u64 = (start..stop).map(|_| {
    let x: f64 = rng.gen();
    let y: f64 = rng.gen();

    if x * x + y * y <= 1.0 { 1 } else { 0 }
  }).sum();

  let print_duration = || {
    let duration = start_time.elapsed().as_millis();
    println!("| {:4} | {:20} | {:20} | {:17} ms |", rank, start, stop, duration);
  };

  if rank == 0 {
    for r in 1..size {
      let mut other_inside = 0;

      world.process_at_rank(r).receive_into(&mut other_inside);

      inside += other_inside;
    }

    print_duration();
  } else {
    print_duration();
    world.process_at_rank(0).synchronous_send(&inside);
  }

  if rank == 0 {
    println!("└──────┴──────────────────────┴──────────────────────┴──────────────────────┘\n");

    let pi = inside as f64 / samples as f64 * 4.0;
    println!("π ≈ {}", pi);
  }
}
