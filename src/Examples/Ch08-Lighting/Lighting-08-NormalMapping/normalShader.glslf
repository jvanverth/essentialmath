varying vec2 uv;
uniform vec4 dirLightPosition;
uniform sampler2D Texture;

void main()
{
	float lighting = clamp(dot(2.0 * (texture2D(Texture, uv).rgb - 0.5), dirLightPosition.xyz), 0.0, 1.0);
    gl_FragColor = vec4(lighting, lighting, lighting, 1);
}
