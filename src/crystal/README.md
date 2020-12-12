This program produces what are called Brownian Crystals, wherein particles
move around randomly until they touch a seed particle and adhere to it.

As of now there are not many shortcuts that are relevant for this program,
it mostly runs itself.

Once it runs out of moving particles it takes a screenshot and saves it as
`Crystal-NNN.png` and reset. This behavior should be controllable in future
versions.

Here's what exists so far:

* A - Toggle drawing the freely moving particles versus just the fixed ones
* R - Reset the crystal
* T - Toggle drawing the text over the crystal
* W - Change walking strategy(More on this later)
* N - Change the neighbor check
* D - Toggle diagonal motion
* F - Toggle saving frames (Good for making animations of the crystal growing)
* O - Save a screenshot

### Walking Strategy

These are rules governing how the particles are moved:

#### Random Walk

Particles have 1/2 chance to move either horizontally or vertically one space.
The expression used to determine which direction they move in either case is `random()%3-1`.

If diagonal movement is enabled, then it also has a chance of moving both directions,
once again determined by the expression previously.

#### Center Nudges

Particles have a 1 in 60 chance of being moved towards the center of the window(this is
surprisingly efficatious). Otherwise it performs a random walk.

### Neighbor Checks

These are the different adjacency checks that exist currently `X` denotes
a space on the grid where `P` is considered adjacent to.

#### 4-cardinal

```
 |X|
X|P|X
 |X|
```

#### 4-diagonal

```
X| |X
 |P|
X| |X
```

#### 8-neighborhood

```
X|X|X
X|P|X
X|X|X
```