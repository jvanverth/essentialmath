This example demonstrates the textured rendering of a cylinder using both
textures based on an image file and on image data created directly in app 
code.  The application allows the user to switch textures on the fly and to
change texture coordinates on the fly.  It also allows the user to select
the desired texture addressing mode in U and V independently.

The key commands are:

i, k - translate in x
j, l - translate in y
u, o - rotate around z axis
p, ; - uniformly scale
Space - reset transforms
t - change texture image
c - change texture coordinates
h - change U texture addressing (wrap/clamp)
v - change V texture addressing (wrap/clamp)
