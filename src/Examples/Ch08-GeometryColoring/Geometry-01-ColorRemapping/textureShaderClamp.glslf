#version 150

uniform sampler2D Texture;
uniform float scale;

in vec2 uv;
in vec4 color;

out vec4 fragColor;

void main()
{
    fragColor = clamp(texture(Texture, uv) * scale,
		vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
}
