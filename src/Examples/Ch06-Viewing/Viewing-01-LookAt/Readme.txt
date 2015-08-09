This code example shows how to build the view matrix to follow a particular object.  

One thing to try: back up the object until the camera is looking right down
on it.  If you manage it, the screen will go to black.  This happens because
the cross product between the up vector and view vector goes to zero.

Controls
--------
ESC: quit
I,J,K,L: translate object in the XY plane
SPACE: reset transforms
