varying vec2 uv;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    uv = gl_MultiTexCoord0.xy;
}
