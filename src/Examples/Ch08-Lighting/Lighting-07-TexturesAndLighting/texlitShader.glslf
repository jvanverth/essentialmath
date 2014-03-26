varying vec4 diffColor;
varying vec4 specColor;
varying vec2 uv;

uniform float splitColor;
uniform sampler2D Texture;

void main()
{
	vec4 tex = texture2D(Texture, uv);
	gl_FragColor = splitColor * (specColor + diffColor * tex) + (1.0 - splitColor) * ((specColor + diffColor) * tex);
}
