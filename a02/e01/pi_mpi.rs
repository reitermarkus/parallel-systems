use std::{env, io::{stdout, Write}, time::Instant};

use mpi::{collective::{CommunicatorCollectives, Root, SystemOperation}, topology::Communicator};
use rand::Rng;

fn main() {
  let samples = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(1_000_000_000);

  let start_time = Instant::now();

  let universe = mpi::initialize().expect("MPI failed to initialize");
  let world = universe.world();
  let size = world.size();
  let rank = world.rank();

  if rank == 0 {
    println!("┌──────┬──────────────────────┬──────────────────────┬──────────────────────┐");
    println!("❘ Rank ❘                Start ❘                 Stop ❘                 Time ❘");
    println!("├──────┼──────────────────────┼──────────────────────┼──────────────────────┤");
  }

  world.barrier();

  let chunk_size = (samples + size as u64 - 1) / size as u64;
  let start = chunk_size * rank as u64;
  let stop = num::clamp(start + chunk_size, 0, samples);

  let mut rng = rand::thread_rng();

  let mut local_inside: u64 = (start..stop).map(|_| {
    let x: f64 = rng.gen();
    let y: f64 = rng.gen();

    if x * x + y * y <= 1.0 { 1 } else { 0 }
  }).sum();

  let print_duration = || {
    let duration = start_time.elapsed().as_millis();
    println!("| {:4} | {:20} | {:20} | {:17} ms |", rank, start, stop, duration);
    stdout().flush().unwrap();

    world.barrier();
  };

  if rank == 0 {
    let mut inside = 0;
    world.process_at_rank(0).reduce_into_root(&mut local_inside, &mut inside, SystemOperation::sum());
    print_duration();

    let pi = inside as f64 / samples as f64 * 4.0;

    println!("└──────┴──────────────────────┴──────────────────────┴──────────────────────┘\n");
    println!("π ≈ {}", pi);
  } else {
    world.process_at_rank(0).reduce_into(&local_inside, SystemOperation::sum());
    print_duration();
  }
}
