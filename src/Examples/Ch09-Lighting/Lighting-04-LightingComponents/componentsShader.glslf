// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#version 150

in vec3 normal;
in vec3 worldPos;
in vec3 viewDir;

out vec4 fragColor;

uniform vec3 pointLightPosition; // in world space
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

lightSampleValues computePointLightValues(in vec3 surfacePosition)
{
    lightSampleValues values;

    vec3 lightVec = pointLightPosition - surfacePosition;

    values.dir = normalize(lightVec);

	float distAtten = 1.0 + dot(lightVec, lightVec);

    values.L = pointLightIntensity / distAtten;

    return values;
}

void main()
{
	lightSampleValues lightValues = computePointLightValues(worldPos);

    fragColor.rgb = materialEmissiveColor
                   + computeAmbientComponent()
                   + computeLitColor(lightValues, worldPos, normalize(normal),
                                     normalize(viewDir));
    fragColor.a = materialDiffuseColor.a;
}

