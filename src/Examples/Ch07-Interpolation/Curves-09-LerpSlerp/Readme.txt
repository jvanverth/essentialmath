This code shows the different between linear interpolation and spherical linear 
interpolation of quaternions.  The green cube uses slerp, the red cube uses lerp.  
The key thing to notice is that the two cubes are not coincident -- the green cube 
outraces the red cube until the middle, when the red cube races ahead.  
You can activate Jonathan Blow's spline-based correction for linear interpolation 
by hitting C, and the linear interpolation speed matches the slerp interpolation much
better.

Controls
--------
ESC: quit
SPACE: reset/stop animation
C: toggle Blow's correction on/off
