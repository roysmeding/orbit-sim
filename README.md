# orbit-sim -- Toy N-body simulator

This project is an N-body simulator, which means it calculates the effect of
gravity between a number of different bodies (planets, stars, asteroids) to
determine their orbits as time goes on.

It was written primarily as a toy for me to practice optimizing code that
performs a lot of floating-point computation. As such, it is quite heavily
optimized for the Sandy Bridge architecture (or later). I run it on an
Intel(R) Core(TM) i5-2500K CPU @ 3.30GHz.

The universe.txt file included provides the starting point for the
calculations. It was based on data from JPL's HORIZONS system, which can be
found [here](http://ssd.jpl.nasa.gov/?horizons).
