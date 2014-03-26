varying vec4 color;

uniform vec4 pointLightPosition; 
uniform vec4 pointLightAttenuation; 
uniform float pointLightIntensity;

struct lightSampleValues
{
	vec3 L;
	float iL;
};

lightSampleValues computePointLightValues(in vec4 surfacePosition)
{
	lightSampleValues values;

	values.L = (pointLightPosition - surfacePosition).xyz;

	float dist = length(values.L);
	values.L = values.L / dist; // normalize

	// Dot computes the 3-term attenuation in one operation
	// k_c * 1.0 + k_l * dist + k_q * dist * dist

	float distAtten = dot(pointLightAttenuation.xyz, 
		vec3(1.0, dist, dist*dist));

	values.iL = pointLightIntensity / distAtten;

	return values;
}

void main()
{
	lightSampleValues lightValues = computePointLightValues(gl_Position);
	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    float lighting = clamp(dot(gl_Normal, lightValues.L) * lightValues.iL, 0.0, 1.0);
    
    color = vec4(lighting, lighting, lighting, 1);
}
