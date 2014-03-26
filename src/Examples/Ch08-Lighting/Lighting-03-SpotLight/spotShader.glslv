varying vec4 color;

struct lightSampleValues
{
	vec3 L;
	float iL;
};

uniform vec4 spotLightPosition; // position with w == 1
uniform float spotLightIntensity;
uniform vec4 spotLightAttenuation; // (k\_c, k\_l, k\_q) 
uniform vec4 spotLightDir; // unit-length 
uniform float spotLightAngleCos;
uniform float spotLightExponent;

lightSampleValues computeSpotLightValues(in vec4 surfacePosition)
{
	lightSampleValues values;
	
	values.L = (spotLightPosition - surfacePosition).xyz;
	
	float dist = length(values.L);

	values.L = values.L / dist; // normalize

	// Dot computes the 3-term attenuation in one operation
	// k_c * 1.0 + k_l * dist + k_q * dist * dist
	float distAtten = dot(spotLightAttenuation.xyz, 
		vec3(1.0, dist, dist*dist));

	float spotAtten = dot(-spotLightDir.xyz, values.L);

	spotAtten = (spotAtten > spotLightAngleCos) 
		? pow(spotAtten, spotLightExponent) : 0.0;

	values.iL = spotLightIntensity * spotAtten / distAtten;

	return values;
}

void main()
{
	lightSampleValues lightValues = computeSpotLightValues(gl_Position);
	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    float lighting = clamp(dot(gl_Normal, lightValues.L) * lightValues.iL, 0.0, 1.0);

    color = vec4(lighting, lighting, lighting, 1);
}
