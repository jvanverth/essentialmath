This example demonstrates an advanced form of per-fragment lighting;
Object-space normal mapping.  A texture containing colors representing
the normal vector in model space is applied to the object, and a per-
fragment lighting computation is done versus this normal and a light
vector transformed into object space.  The geometry in this example is
a single quad comprised of two textures.

Thanks to bricksntiles.com for the use of the color and normal maps.

Key controls are:

a, d - rotate the lighting direction vector.
