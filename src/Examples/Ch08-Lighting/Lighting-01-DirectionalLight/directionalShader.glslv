varying vec4 color;

uniform vec4 dirLightPosition; 
uniform float dirLightIntensity;

struct lightSampleValues
{
	vec3 L;
	float iL;
};

lightSampleValues computeDirLightValues()
{
	lightSampleValues values;

	values.L = dirLightPosition.xyz;

	values.iL = dirLightIntensity;

	return values;
}

void main()
{
	lightSampleValues lightValues = computeDirLightValues();
	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    float lighting = clamp(dot(gl_Normal, lightValues.L) * lightValues.iL, 0.0, 1.0);
    
    color = vec4(lighting, lighting, lighting, 1);
}
