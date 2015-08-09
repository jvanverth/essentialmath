This demo shows how colors outside of the unit cube [0, 1] can be remapped
back into the unit cube.  The texture colors can be scaled outside of the
unit cube via key controls, and the conversion method (downscaling by the
max component or direct clamping) can be switched.  Note the "white out"
effects of using clamping.

The key commands are:

i, k - translate in x
j, l - translate in y
u, o - rotate around z axis
p, ; - uniformly scale
Space - reset transforms
-, = - scale texture colors down/up
t - switch texture
s - switch color remapping mode
