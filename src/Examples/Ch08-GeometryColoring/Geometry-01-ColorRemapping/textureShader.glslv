layout(location = COLOR) in vec4 inColor;
layout(location = POSITION) in vec4 position;
layout(location = TEXCOORD0) in vec2 texCoord0;

uniform mat4 IvModelViewProjectionMatrix;

out vec2 uv;
out vec4 color;

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;
    color = inColor;
    uv = texCoord0.xy;
}
