layout(location = POSITION) in vec4 position;
layout(location = NORMAL) in vec3 inNormal;

out vec3 normal;
out vec3 worldPos;

uniform mat4 IvModelViewProjectionMatrix;
uniform mat4 IvNormalMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;
    worldPos = vec3(modelMatrix * position);
    normal = vec3(IvNormalMatrix * vec4(inNormal, 0));
}
