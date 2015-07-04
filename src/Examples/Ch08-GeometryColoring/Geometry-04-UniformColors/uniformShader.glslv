uniform mat4 IvModelViewProjectionMatrix;

layout(location = POSITION) in vec3 position;
void main()
{
    gl_Position = IvModelViewProjectionMatrix*vec4(position,1.0);
}
