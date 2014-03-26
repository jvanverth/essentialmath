varying vec2 uv;
varying vec4 color;
uniform sampler2D Texture;
uniform float scale;

void main()
{
    gl_FragColor = texture2D(Texture, uv) * scale;
    
    float maxVal = max(gl_FragColor.r, gl_FragColor.g);
    maxVal = max(maxVal, gl_FragColor.b);
    maxVal = max(maxVal, 1.0);
    
    gl_FragColor /= maxVal;
}
