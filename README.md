Hi, this is our little collection of relatively simple demos we made for the purposes of learning
raylib properly.

## List of programs included

1. [nbody](src/nbody/README.md)
  
   ![Nbody simulation](images/nbody.gif?raw=true "An nbody simulator")

2. [rain](src/rain/README.md)
  
   ![Rain animation](images/rain.gif?raw=true "Basic rain falling")

3. flame
  
   (There would be an animation here, but it's a bit spastic to really demonstrate at the moment)

4. flame2
  
   ![Flame particles](images/flame2.gif "A bunch of particles that roughly resemble a fire")

5. [Crystal](src/crystal/README.md)
  
   A brownian crystal generator.
  
   ![crystal](images/crystal.gif)


# Building

## Requirements

* CMake Version >= 3.12
* A functioning C99 compiler with openmp support
* A functioning pthreads library
* POSIX compliance

   Sorry, but we really don't feel like learning windows programming so that we can find
   the nearest equivalent to nanosleep. If we can help you get it to work on WSL/cygwin or
   whatever you have then file an issue, we're not familiar with building it under windows
   and we lack a machine to test it on that has windows running on it.

## Building

Clone the repository:

```
$> git clone https://github.com/epsilon-phase/raylib-experiments
```

Fetch the submodules

```
$> cd raylib-experiments
$> git submodules --init --recursive
```

Make the build directory

```
mkdir build
cd build
```

Run CMake to configure it

```
$> cmake ..
```

Build it

```
$> make
```

Run any of the following as you prefer :)

```
./crystal
./nbody
./rain
./flame2
```