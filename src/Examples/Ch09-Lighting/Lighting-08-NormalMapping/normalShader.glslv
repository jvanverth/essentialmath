layout(location = POSITION) in vec4 position;
layout(location = TEXCOORD0) in vec2 texCoords;

out vec2 uv;

uniform mat4 IvModelViewProjectionMatrix;

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;

    uv = texCoords;
}
