// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#version 150

in vec3 normal;

out vec4 fragColor;

uniform vec3 dirLightPosition;
uniform vec3 dirLightRadiance;

struct lightSampleValues
{
	vec3 dir;
	vec3 L;
};

lightSampleValues computeDirLightValues()
{
	lightSampleValues values;

	values.dir = dirLightPosition;

	values.L = dirLightRadiance;

	return values;
}

void main()
{
	lightSampleValues lightValues = computeDirLightValues();

    vec3 lighting = clamp(dot(normalize(normal), lightValues.dir), 0.0, 1.0) * lightValues.L;
    
    fragColor = vec4(lighting.r, lighting.g, lighting.b, 1.0);
}
