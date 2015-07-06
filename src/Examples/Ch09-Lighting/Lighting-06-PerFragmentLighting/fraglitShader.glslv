layout(location = POSITION) in vec4 position;
layout(location = NORMAL) in vec3 inNormal;

out vec3 normal;

uniform mat4 IvModelViewProjectionMatrix;

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;
    
    normal = inNormal;
}
