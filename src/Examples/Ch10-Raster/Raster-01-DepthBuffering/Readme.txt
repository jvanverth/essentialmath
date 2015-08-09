This example shows the effects of depth buffering.  The left teapot (red) is
not depth buffered (it neither writes nor tests the depth buffer).  The right
teapot (blue) is fully depth buffered.  When rotating the two teapots in sync
note how the non-depth-buffered teapot shows artifacts, with the spout and
handle not occluding correctly.  In addition, the gray plane (depth buffered)
is set at a fixed depth.  Note how it correctly intersects the depth buffered
teapot, but has no effect on the non-depth-buffered teapot.  Even when the
non-depth-buffered teapot is far behind the plane, it is still drawn.

The key commands are:

i, k - translate in x
j, l - translate in y
u, o - rotate around x axis
p, ; - uniformly scale
Space - reset transforms
