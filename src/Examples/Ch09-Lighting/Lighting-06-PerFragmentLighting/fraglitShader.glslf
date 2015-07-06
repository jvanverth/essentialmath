#version 150

in vec3 normal;

out vec4 fragColor;

uniform vec3 dirLightPosition;

void main()
{
    float lighting = clamp(dot(normalize(normal), dirLightPosition), 0.0, 1.0);
    
    fragColor = vec4(lighting, lighting, lighting, 1.0);
}
