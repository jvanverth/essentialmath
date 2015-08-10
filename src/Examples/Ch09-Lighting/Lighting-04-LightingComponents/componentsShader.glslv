// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

layout(location = POSITION) in vec4 position;
layout(location = NORMAL) in vec3 inNormal;

out vec3 normal;
out vec3 worldPos;
out vec3 viewDir;

uniform mat4 IvModelViewProjectionMatrix;
uniform mat4 IvNormalMatrix;
uniform mat4 modelMatrix;
uniform vec3 viewPosition;   // in world space

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;
    worldPos = vec3(modelMatrix * position);
    normal = vec3(IvNormalMatrix * vec4(inNormal, 0));
    viewDir = viewPosition - worldPos;
}
