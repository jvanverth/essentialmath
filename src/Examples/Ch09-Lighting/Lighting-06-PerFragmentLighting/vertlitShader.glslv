uniform vec3 dirLightPosition;

layout(location = POSITION) in vec4 position;
layout(location = NORMAL) in vec3 inNormal;

out vec4 color;

uniform mat4 IvModelViewProjectionMatrix;

void main()
{
    float lighting = clamp(dot(normalize(inNormal), dirLightPosition), 0.0, 1.0);
    color = vec4(lighting, lighting, lighting, 1.0);
    
    gl_Position = IvModelViewProjectionMatrix * position;
}
