In this demo we show different Perlin noise shaders: a sky and marble texture. Both are created in the fragment shader, by passing the local position in and generating a noise value which is used to blend two colors. 

Note that these are quite slow; it would probably be better to do a lookup via a texture map rather than use the noise function.

Also, due to time constraints, only OpenGL is currently supported.  D3D requires a special kind of shader and texture creation for noise() support -- look on the website for updates.


Controls
--------
ESC: quit
I,J,K,L: translate in the XY plane
;,P: scale down, up
U,O: rotate around Z axis
S: swap shaders
