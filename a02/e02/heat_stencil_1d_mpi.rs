use std::{env, mem, process::exit, time::Instant};

use mpi::{point_to_point::{Destination, Source}, request::WaitGuard, topology::Communicator};

fn main() {
  let room_size: usize = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(2000);

  let start_time = Instant::now();

  let universe = mpi::initialize().expect("MPI failed to initialize");
  let world = universe.world();
  let size = world.size();
  let rank = world.rank();

  let time_steps = room_size * 500;
  if rank == 0 {
    println!("Computing heat-distribution for room size {} for {} timesteps\n", room_size, time_steps);
  }

  let chunk_size: usize = (room_size + (size - 1) as usize) / size as usize;
  let last_chunk_size: usize = if room_size % chunk_size == 0 { chunk_size } else { room_size % chunk_size};
  let buffer_size: usize = if rank == size - 1 { last_chunk_size } else { chunk_size };

  // Skip this rank if the start index is higher than the room size.
  if rank as usize * chunk_size >= room_size {
    return;
  }

  let mut buffer_a: Vec<f64> = vec![273.0; buffer_size];
  let mut buffer_b: Vec<f64> = vec![0.0; buffer_size];

  let source_x: usize = room_size / 4;
  let source_rank: usize = source_x / chunk_size;
  let source_index: usize = source_x % chunk_size;

  if rank as usize == source_rank {
    println!("Heat source is at rank {}, index {}.", source_rank, source_index);
    buffer_a[source_index] += 60.0;
  }

  // Propagate the temperature in each time step.
  for _ in 0..time_steps {
    mpi::request::scope(|scope| {
      if rank > 0 {
        WaitGuard::from(world.process_at_rank(rank - 1).immediate_send_with_tag(scope, &buffer_a[0], 1));
      }

      if rank < size - 1 {
        WaitGuard::from(world.process_at_rank(rank + 1).immediate_send_with_tag(scope, &buffer_a[chunk_size - 1], 2));
      }
    });

    for i in 0..buffer_size {
      // The center stays constant (the heat is still on).
      if rank as usize == source_rank && i == source_index {
        buffer_b[i] = buffer_a[i];
        continue;
      }

      // Get temperature at current position.
      let temp_current = buffer_a[i];

      // Get temperatures from left neighbor cell.
      let temp_left = if i == 0 {
        if rank == 0 {
          temp_current
        } else {
          let mut temp_left_buf = 0.0;
          world.process_at_rank(rank - 1).receive_into_with_tag(&mut temp_left_buf, 2);
          temp_left_buf
        }
      } else {
        buffer_a[i - 1]
      };

      // Get temperatures from right neighbor cell.
      let temp_right = if i == buffer_size - 1 {
        if rank as usize * chunk_size + i >= room_size - 1 {
          temp_current
        } else {
          let mut temp_right_buf = 0.0;
          world.process_at_rank(rank + 1).receive_into_with_tag(&mut temp_right_buf, 1);
          temp_right_buf
        }
      } else {
        buffer_a[i + 1]
      };

      buffer_b[i] = temp_current + 0.2 * (temp_left + temp_right + (-2.0 * temp_current));
    }

    mem::swap(&mut buffer_a, &mut buffer_b);
  }

  if rank == 0 {
    buffer_a.resize(room_size, 0.0);

    for i in 1..size {
      let start = chunk_size * i as usize;
      let end = start + if i == size - 1 { last_chunk_size } else { chunk_size };

      // Break if we already received the whole room.
      if start >= room_size {
        break
      }

      world.process_at_rank(i).receive_into_with_tag(&mut buffer_a[start..end], 3);
    }
  } else {
    world.process_at_rank(0).send_with_tag(&buffer_a[..], 3);
    return;
  }

  let duration = start_time.elapsed().as_millis();

  print_temperature(&buffer_a);
  println!(" final");

  println!("Duration: {} ms", duration);

  print!("Verification: ");

  for temp in buffer_a.into_iter() {
    if temp < 273.0 || temp > 273.0 + 60.0 {
      println!("FAILED");
      exit(1);
    }
  }

  println!("OK");
}

fn print_temperature(temperatures: &[f64]) {
  let colors = [' ', '.', '-', ':', '=', '+', '*', '^', 'X', '#', '%', '@'];

  let max = 273.0 + 30.0;
  let min = 273.0 +  0.0;

  let resolution = 120;
  let step_size = temperatures.len() / resolution;

  print!("|");

  for i in 0..resolution {
    let mut max_temp = 0.0;

    for x in (step_size * i)..(step_size * (i + 1)) {
      if max_temp < temperatures[x] {
        max_temp = temperatures[x];
      };
    }

    // println!("MAX_TEMP = {}", max_temp);

    let c = (((max_temp - min) / (max - min)) * colors.len() as f64) as isize;
    let c = if c >= colors.len() as isize { colors.len() - 1 } else if c < 0 { 0 } else { c as usize };

    print!("{}", colors[c]);
  }

  print!("|");
}
