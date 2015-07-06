#version 150

in vec3 normal;
in vec3 worldPos;
in vec3 viewDir;

out vec4 fragColor;

uniform vec3 pointLightPosition[2]; // in world space
uniform vec3 pointLightIntensity;

uniform vec3 ambientLightColor;

uniform vec3 materialAmbientColor;
uniform vec3 materialEmissiveColor;
uniform vec4 materialDiffuseColor;
uniform vec4 materialSpecularColorExp;

struct lightSampleValues
{
	vec3 dir;
	vec3 L;
};

vec3 computeAmbientComponent()
{
    return ambientLightColor * materialAmbientColor;
}

// surfaceNormal is assumed to be unit-length

vec3 computeDiffuseBRDF(in vec3 surfaceNormal,
                        in lightSampleValues light)
{
    return materialDiffuseColor.rgb;
}

vec3 computeSpecularBRDF(in vec3 surfaceNormal,
						 in vec3 surfacePosition,
                         in vec3 viewDir,
                         in lightSampleValues light)
{
    vec3 halfVector = normalize(viewDir + light.dir);

    float nDotH = clamp(dot(surfaceNormal, halfVector), 0.0, 1.0);

    return materialSpecularColorExp.rgb
            * pow(nDotH, materialSpecularColorExp.a);
}

vec3 computeLitColor(in lightSampleValues light,
					 in vec3 surfacePosition,
					 in vec3 surfaceNormal,
                     in vec3 viewDir)
{
    vec3 brdf = computeDiffuseBRDF(surfaceNormal, light)
              + computeSpecularBRDF(surfaceNormal, surfacePosition, viewDir, light);

    return light.L * brdf * clamp(dot(surfaceNormal, light.dir), 0.0, 1.0);
}

lightSampleValues computePointLightValues(in vec3 surfacePosition, int i)
{
    lightSampleValues values;

    vec3 lightVec = pointLightPosition[i] - surfacePosition;

    values.dir = normalize(lightVec);

	float distAtten = 1.0 + dot(lightVec, lightVec);

    values.L = pointLightIntensity / distAtten;

    return values;
}

void main()
{

    fragColor.rgb = materialEmissiveColor
                   + computeAmbientComponent();
    vec3 unitViewDir = normalize(viewDir);
    vec3 unitNormal = normalize(normal);
    for (int i = 0; i < 2; ++i)
    {
	    lightSampleValues lightValues = computePointLightValues(worldPos, i);
        fragColor.rgb += computeLitColor(lightValues, worldPos, unitNormal, unitViewDir);
    }
    fragColor.a = materialDiffuseColor.a;
}

