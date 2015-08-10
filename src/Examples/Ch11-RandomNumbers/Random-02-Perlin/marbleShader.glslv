// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

uniform mat4 IvModelViewProjectionMatrix;
uniform mat4 IvNormalMatrix;
uniform vec4 IvLightDirection;
uniform vec4 IvLightAmbient;
uniform vec4 IvLightDiffuse;
uniform vec4 IvDiffuseColor;

layout(location = NORMAL) in vec3 IvNormal;
layout(location = POSITION) in vec3 IvPos;

out vec3 localPos;
out vec4 color;

void main()
{
    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);
    localPos = IvPos;
    vec4 transNormal = normalize(IvNormalMatrix * vec4(IvNormal,0.0));
    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);
    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;
    color = IvDiffuseColor*lightValue;
}
