// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

layout(location = POSITION) in vec4 position;
layout(location = TEXCOORD0) in vec2 texCoords;

out vec2 uv;
out vec3 lightDir;

uniform mat4 IvModelViewProjectionMatrix;
uniform mat4 IvNormalMatrix;
uniform vec3 dirLightDirection;

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;

    uv = texCoords;
	// transform light direction from world space to object space
	lightDir = vec3(vec4(dirLightDirection, 0.0) * IvNormalMatrix);
}
