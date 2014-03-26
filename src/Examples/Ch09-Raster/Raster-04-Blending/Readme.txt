This example demonstrates the textured rendering of two copies of a cylinder.
The first, inner cylinder is rendered as opaque with the soup can texture. The
second, outer cylinder is rendered using one of a set of three different
textures: 
1) a texture with solid, patterned RGB components, and a fence-shaped alpha
channel, leading to the appearance of a cage when alpha blending is used
2) a texture with a white background and dark dots, with an alpha component
that is opaque where the texture is dark.  This texture is interesting with
several blending modes, including modes that ignore the alpha channel.
3) a texture whose RGB components are the inverse of the above texture.

The key commands are:

i, k - translate in x
j, l - translate in y
u, o - rotate around z axis
p, ; - uniformly scale
Space - reset transforms
t - change blending texture image
b - change blending mode (alpha blending, additive, multiply, none)
