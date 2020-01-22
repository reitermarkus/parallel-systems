use Random;

config const samples = 10**9;
config const threads = 1;

iter monteCarlo(samples: int(64), t: int = 0) {
  var randomStream = new owned RandomStream(real, t, parSafe=false);

  for i in vectorizeOnly(0..#samples) {
    var x = randomStream.getNext();
    var y = randomStream.getNext();
    yield if x * x + y * y <= 1.0 then 1 else 0;
  }
}

iter monteCarlo(param tag: iterKind, samples: int(64), t: int = none) where tag == iterKind.standalone {
  coforall t in 0..#threads {
    for e in monteCarlo(samples / threads, t) do
      yield e;
  }
}

var inside = 0;

coforall loc in Locales with (+ reduce inside) {
  on loc {
    forall p in monteCarlo(samples / numLocales) with (+ reduce inside) do
      inside += p;
  }
}

var pi = inside:real / samples:real * 4.0;

writeln("π = ", pi);
