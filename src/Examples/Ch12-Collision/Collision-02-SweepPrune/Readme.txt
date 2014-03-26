This demo shows how to build a simple collision system. The yellow spheres are our objects. 
They highlight as red when a collision is detected.  The lines at the bottom represent the 
extents used by the sweep-and-prune method: maroon is the start of an object extent and
cyan is the end.

In this case, we are using the sweep-and-prune method.  A list is created, storing the min
and max x extents for each object, with a pointer to each object.  The list is sorted by 
the x values.  Then we go through the extent list.  If it's a min extent, we check for 
collisions in the active list, and then add the object to the active list.  Otherwise we 
remove the object from the active list.

The key commands are:

ESC - quit
