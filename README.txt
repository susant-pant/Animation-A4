# Animation-A4
Reynolds' model of flocks, herds and schools

# Controls
W , S , A , D : Move Camera position
Mouse: Change Camera direction

# Collaborators
None

# Sources
Code : Jeremy''s OpenGL Boilerplate Code
Original Paper : http://www.cs.toronto.edu/~dt/siggraph97-course/cwr87/

# To Run
Run command 'make && ./test.o' in terminal, in directory containing makefile

# Notes
I didn't have the time to implement Flock Centering or Collisions or anything really exciting, so unfortunately it's not a very exciting simulation.

Also I directly change the velocity of the boids instead of changing it using an acceleration value, so this causes the boids to look like they are constantly twitching.

The boids get bluer as they go farther out, and whiter as they come closer towards the camera.

There's a big spherical boundary that the boids 'bounce' away from when they get to close. Since I do this by directly changing velocity, the boids look pretty crazy when they start flocking near the edge of the sphere.
