In this demo we show different noise shaders: a sky and marble texture. 
Both are created in the fragment shader, by passing the local position in and 
generating a noise value which is used to blend two colors. 

The noise function used is not classic Perlin noise, but simplex noise as it
is faster and more symmetric. The implementation is by Ian McEwan of Ashima Arts.
See https://github.com/ashima/webgl-noise for more details.

Controls
--------
ESC: quit
I,J,K,L: translate in the XY plane
;,P: scale down, up
U,O: rotate around Z axis
S: swap shaders
