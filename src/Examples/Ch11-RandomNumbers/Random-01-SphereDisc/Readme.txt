In this demo, we show various methods for generating random distributions: two on a sphere and two on a disc.  In the first sphere and disc methods we try a slightly incorrect approach that leads to clumping in the "corners" in the sphere case and in the center in the disc case.  In the second set we correct those.  In the sphere case the correction is to do rejection sampling to remove  points outside of the sphere.  In the disc case the correction is to take the square root of the random radius before using it.

Note that the disk looks as if it's a dome.  This is an optical illusion due to the viewing angle.

Controls
--------
ESC: quit
;,P: scale down, up
U,O: rotate around Z axis
M: switch generation method for random points
