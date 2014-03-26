Same demo as Chapter 3 - Separate.  The transform is broken into separate 
scale, rotation and translation components which are combined at render time.
In this case we have secretly replaced this person's coffee,
er, 3x3 rotation matrix with a quaternion.

Controls
--------
ESC: quit
I,J,K,L: translate in the XY plane
;,P: scale down, up
U,O: rotate around Z axis
