This example shows the effects of non-mipmapped rendering, mipmapping without
interpolation across mipmap levels, and mipmapping with trilinear interpolation.
Initially, the demo uses bilinear filtering.

The demo shows two textured quads, rendered with different textures, but using
the same filtering mode.  The texture quads are twice as tall as they are wide
and are initially rotated so that the tops are more distant from the viewer 
than the bottoms.

The texture on the right is a simple checkerboard, mipmapped normally

The texture on the left is rather special.  Each mipmap level is a solid color
with each mipmap level being a different color from the other levels.  As a
result, this quad shows very clearly which mipmap levels are being used on the
quad.  The two quads have the same texture coordinates and use textures that
are the same size, so each quad should be using the same mipmap level(s) at
any given time. NOTE: in D3D9, automatic mipmapping is currently damaging this
texture - the effect only works in OpenGL.

Users should cycle through the filtering modes, to see how they change the
rendered appearance of the textures.  The quads are initially bilerped, and
pressing the 'f' key cycles to mipmapping without trilerp, mipmapping with
trilerp, and then back to bilerp.

Also, the user can change the orientation of the quads together, using the
'u' and 'o' keys, which allow the polygons to be viewed aligned or oblique to
the screen.  Note that the more oblique the quads are with respect to the 
camera, the greater the number of mipmap levels that are used across each
quad.

Finally, note the extreme aliasing in bilerp mode, and the obvious 
transitions between mipmap levels in non-trilerped mipmapping

The key commands are:

u, o - rotate around x axis
p, ; - uniformly scale
Space - reset transforms
f - cycle filtering modes
