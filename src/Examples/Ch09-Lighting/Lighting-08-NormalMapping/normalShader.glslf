#version 150

in vec2 uv;
out vec4 fragColor;

uniform vec3 dirLightPosition;
uniform sampler2D Texture;

void main()
{
    vec3 normal = 2.0 * (texture(Texture, uv).rgb - vec3(0.5));
    float lighting = clamp(dot(normal, dirLightPosition), 0.0, 1.0);

    fragColor = vec4(lighting, lighting, lighting, 1.0);
}
