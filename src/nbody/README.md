#Nbody

A basic multithreaded 2d nbody simulator.


## Controls

* `Left/Right` Decrease/increase number of bodies in the simulation
* `R` Reinitialize the simulation
* `T` Toggle threading, for small particle counts multithreading tends to provide unnecessary
  overhead.
* `Up`/`Down` Increase or decrease simulation speed
* `-`/`+` Decrease or increase the maximum mass a particle will be spawned with
* `/`/`h` Toggle the in-program help
* `a` Toggle drawing of acceleration vectors
* `s` Toggle showing simulation statistics

## Statistics

* **Largest Mass**
  The mass of the heaviest body in the simulation
* **Heaviest Radius**
  The radius of the heaviest object in the simulation.
* **Collision Count**
  The number of collisions that have happened since either the start of the program or the simulation was reset
* **Simulated Time**
  The amount of time that has been simulated in seconds. (Each step is 1/60th of a second)
* **Wall Time**
  The length of actual time since the simulation was reset or the program was started.
* **Dilation Factor**
  The number of seconds in simulation that pass for each second of the simulation