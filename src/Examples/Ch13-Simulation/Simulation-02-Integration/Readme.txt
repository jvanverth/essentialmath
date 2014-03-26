This demo shows the effect of different integration methods on a simple 
orbiting object.  You can toggle between seven different integration methods: Euler's method, midpoint method, Runge-Kutta 4, velocity Verlet, a naive implicit method, a simple predictor-corrector, and symplectic Euler.  Note how the Euler becomes more unstable, and the midpoint as well but less so.  The implicit method, on the other hand, spirals inward.  The symplectic method is the most tolerant of all of them.

Controls
--------

M: switch integration method
SPACE:   resets demo
ESC, Q:  quit
