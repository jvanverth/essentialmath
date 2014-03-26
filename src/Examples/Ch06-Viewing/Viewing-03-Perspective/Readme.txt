This code example shows how to build the OpenGL projection matrix, as in the call
gluPerspective().  Controls allow you to change the field of view, near plane, and far plane,
and see the effect.

Try setting the field of view very small or very large.  As it goes past 0 or 180, 
the result is to invert the view.

Controls
--------
ESC: quit
q, a: decrease, increase field of view
w, s: move near plane out and in
e, d: move far plane out and in
SPACE: reset transforms
