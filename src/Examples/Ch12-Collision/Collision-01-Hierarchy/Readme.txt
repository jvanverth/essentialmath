This demo shows the power of a simple scene graph, which allows for quick and
easy creation, management and rendering of hierachical scenes such as the
articulated submarine used herein.  The demo allows the user to move and rotate
the submarine.  In addition, the rudder, dive planes and periscope can be 
independently articulated, as well

This demo also displays the bounding hierarchy for the submarine, showing how 
the different levels of bounding objects are constructed and relate to one
another.  The leaf level of bounding capsules is displayed in orange, and
the internal level of bounding spheres is displayed in yellow.  Each can be 
toggled on and off.

Thanks to Mike Cosner for assistance with the submarine model.

The key commands are:

ESC - quit
j, l - translate sub in x
i, k - translate sub in y
z, c - rotate sub around z
a, d - rotate rudder around z
w, s - rotate dive planes vertically
r, f - move periscope up and down
q  - toggle hierarchy bounds display
e  - toggle leaf bounds display
space - reset all transforms
