varying vec4 diffColor;
varying vec4 specColor;
varying vec2 uv;

uniform vec4 viewPosition; 

uniform vec4 pointLightPosition; 
uniform vec4 pointLightAttenuation; 
uniform float pointLightIntensity;

uniform vec4 materialAmbientColor;
uniform vec4 materialDiffuseColor;
uniform vec4 materialSpecularColor;
uniform float materialSpecularExp;

uniform vec4 lightAmbDiffSpec;
uniform vec4 lightColor;

struct lightSampleValues
{
	vec3 L;
	float iL;
};

vec3 computeAmbientComponent(in lightSampleValues light)
{
	return light.iL * (lightColor.rgb * lightAmbDiffSpec.x)
		* materialAmbientColor.rgb;
}

// surfaceNormal is assumed to be unit-length

vec3 computeDiffuseComponent(in vec3 surfaceNormal, 
							 in lightSampleValues light)
{
	return light.iL * (lightColor.rgb * lightAmbDiffSpec.y) 
					* materialDiffuseColor.rgb
					* max(0.0, dot(surfaceNormal, light.L));
}

vec3 computeSpecularComponent(in vec3 surfaceNormal, 
							  in vec4 surfacePosition,
							  in lightSampleValues light)
{
	vec3 viewVector = normalize(viewPosition.xyz - surfacePosition.xyz);
	vec3 reflectionVector 
		= 2.0 * dot(light.L, surfaceNormal) * surfaceNormal 
			- light.L;

	return (dot(surfaceNormal, light.L) <= 0.0) 
		? vec3(0.0, 0.0, 0.0) 
		: (light.iL * (lightColor.rgb * lightAmbDiffSpec.z) 
		* materialSpecularColor.rgb
		* pow(max(0.0, dot(reflectionVector, viewVector)), 
		materialSpecularExp));
}

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
    
    diffColor.rgb = computeAmbientComponent(lightValues)
		+ computeDiffuseComponent(gl_Normal, lightValues);
		
	specColor.rgb = computeSpecularComponent(gl_Normal, gl_Vertex, lightValues);

    uv = gl_MultiTexCoord0.xy;
}
