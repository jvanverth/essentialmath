#version 150

uniform vec4 constColor;

in vec4 color;
out vec4 fragColor;

void main()
{
    fragColor = color;
}
