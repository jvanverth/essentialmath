varying vec3 normal;

uniform vec4 dirLightPosition;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	normal = gl_Normal;    
}
