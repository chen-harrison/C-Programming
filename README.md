# TriBot Programming Challenges

<sup>(NOTE: Course and instructor name omitted at his request, likely to prevent future students from finding and referencing this repository for solutions)</sup>

These programs all revolve around the use of a wedge-shaped simulated robot, and either build on each other or use a similar infrastructure to perform various tasks.

## Robot:
### *collision*
From ``polygons.csv``, takes in the vertices (up to 16), rotation, and translation of two polygons and determines whether they "collide", which we define as either intersecting or having one shape nested inside of the other.

### *braitenberg*
Simulates the behavior of a [Braitenberg vehicle](https://en.wikipedia.org/wiki/Braitenberg_vehicle) in an environment with three lamps, complete with collision detection and resolution, and visualizes it in the form of a bitmap image:

<img src="media/braitenberg.gif" width="480">

### *chase*
A runner and chaser robot are placed in a maze layout. The runner performs one of three randomized actions: increase forward velocity, increase counter-clockwise rotational velocity, or do nothing. The chaser, whose goal is to catch the runner, has those same three actions, and a fourth that allows it to increase clockwise rotational velocity. It uses a tree search with a depth of 4 (aka 256 unique outcomes) to determine its best action, with a scoring system that rewards minimal distance to the chaser and high velocities. Different start positions for the chaser robot yield different outcomes (i.e. how many time steps necessary to catch the runner).

<img src="media/chase.gif" width="480">

### *manual*
Takes in keyboard input to make the robot either move forward or rotate.

<img src="media/manual.gif" width="480">

### *potential*
Similar to the robot chase problem, a potential field is used to guide the chaser to the runner. Makeshift GUI is created in the terminal to modify the values pertaining runner start location, maximum velocity, and weight of runner distance and obstacle distances. Number of time steps before the chaser catches the runner is printed in the terminal as well.

<img src="media/potential.png" width="480">
<img src="media/potential.gif" width="480">

