This example demonstrates 3-component lighting applied to a 3x3 grid of 
spheres.  The light location may be moved via keys, and the distance 
attenuation function may also be changed. The amount of ambient, diffuse and
specular lighting may also be adjusted.  Each component is a different color
in order to make the effects more obvious. The lighting is done per-vertex 
in a vertex shader.

The key commands are:

i, k - translate in x
j, l - translate in y
Space - reset transforms
0, 1, 2 - set the distance attenuation (constant, linear, quadratic)
a, q - decrease/increase ambient component
s, w - decrease/increase diffuse component
d, e - decrease/increase specular component
