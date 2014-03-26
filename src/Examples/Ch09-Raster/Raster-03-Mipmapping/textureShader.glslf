varying vec2 uv;
uniform sampler2D Texture;

void main()
{
    gl_FragColor = texture2D(Texture, uv);
}
