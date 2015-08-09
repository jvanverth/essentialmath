This example demonstrates basic positional (or local) lighting applied 
to a 3x3 grid of spheres.  The light location may be moved via keys, and
the distance attenuation function may also be changed. The lighting is 
done per-vertex in a vertex shader.

The key commands are:

i, k - translate in x
j, l - translate in y
0, 1, 2 - set the distance attenuation (constant, linear, quadratic)
Space - reset transforms
