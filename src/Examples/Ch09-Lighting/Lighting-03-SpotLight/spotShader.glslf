// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#version 150

in vec3 normal;
in vec3 worldPos;

out vec4 fragColor;

uniform vec3 spotLightPosition; // in world space
uniform vec3 spotLightIntensity;
uniform vec3 spotLightAttenuation;
uniform vec3 spotLightDir; // unit-length, in world space
uniform vec3 spotLightCosOuterInner;

struct lightSampleValues
{
	vec3 dir;
	vec3 L;
};

lightSampleValues computeSpotLightValues(in vec3 surfacePosition)
{
    lightSampleValues values;

    vec3 lightVec = spotLightPosition - surfacePosition;
	float dist = length(lightVec);

    values.dir = normalize(lightVec);

	// Dot computes the 3-term attenuation in one operation
	// k_c * 1.0 + k_l * dist + k_q * dist * dist

	float distAtten = dot(spotLightAttenuation,
		vec3(1.0, dist, dist*dist));

	float spotAtten = dot(-spotLightDir, values.dir);

	spotAtten = smoothstep(spotLightCosOuterInner.x, spotLightCosOuterInner.y, spotAtten);
        
	values.L = spotLightIntensity * spotAtten / distAtten;

    return values;
}

void main()
{
	lightSampleValues lightValues = computeSpotLightValues(worldPos);

    vec3 lighting = clamp(dot(normalize(normal), lightValues.dir), 0.0, 1.0) * lightValues.L;
    
    fragColor = vec4(lighting, 1.0);
}
