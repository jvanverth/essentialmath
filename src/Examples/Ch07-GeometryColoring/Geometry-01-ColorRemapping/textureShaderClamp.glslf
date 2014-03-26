varying vec2 uv;
varying vec4 color;
uniform sampler2D Texture;
uniform float scale;

void main()
{
    gl_FragColor = clamp(texture2D(Texture, uv) * scale, 
		vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
}
