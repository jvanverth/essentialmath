varying vec2 uv;
varying vec4 color;
uniform sampler2D Texture;

void main()
{
    gl_FragColor = texture2D(Texture, uv) * color;
}
