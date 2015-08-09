This simple example shows the difference between per-vertex and per-fragment
lighting by engineering a contrived, obvious case.  The geometry is a single
quad built from two triangles.  The lighting is set up so that a bright
highlight should appear in the center of the quad.  However, vertex lighting
can only reproduce highlights that happen to fall at the vertices.  Thus,
switching this demo to per-vertex lighting will make the highlight 
dissappear.

The key controls are:

a, d - rotate light direction
f - fragment lighting
v - vertex lighting
