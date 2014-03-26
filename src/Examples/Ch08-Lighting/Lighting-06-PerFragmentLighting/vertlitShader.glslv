varying vec4 color;

uniform vec4 dirLightPosition;

void main()
{
	float lighting = clamp(dot(gl_Normal, dirLightPosition.xyz), 0.0, 1.0);
	color.rgb = vec3(lighting, lighting, lighting);    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
