#version 150

in vec3 normal;
in vec3 worldPos;

out vec4 fragColor;

uniform vec3 pointLightPosition; // in world space
uniform vec3 pointLightIntensity;
uniform vec3 pointLightAttenuation;

struct lightSampleValues
{
	vec3 dir;
	vec3 L;
};

lightSampleValues computePointLightValues(in vec3 surfacePosition)
{
    lightSampleValues values;

    vec3 lightVec = pointLightPosition - surfacePosition;
	float dist = length(lightVec);

    values.dir = normalize(lightVec);

	// Dot computes the 3-term attenuation in one operation
	// k_c * 1.0 + k_l * dist + k_q * dist * dist

	float distAtten = dot(pointLightAttenuation,
		vec3(1.0, dist, dist*dist));

    values.L = pointLightIntensity / distAtten;

    return values;
}

void main()
{
	lightSampleValues lightValues = computePointLightValues(worldPos);

    vec3 lighting = clamp(dot(normal, lightValues.dir), 0.0, 1.0) * lightValues.L;
    
    fragColor = vec4(lighting.r, lighting.g, lighting.b, 1.0);
}
