// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#version 150

in vec2 uv;
in vec3 lightDir;
out vec4 fragColor;

uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;

void main()
{
    vec3 normal = normalize(texture(NormalTexture, uv).rgb - vec3(0.5));
    float lighting = clamp(dot(normal, lightDir), 0.0, 1.0);

    fragColor = lighting*texture(ColorTexture, uv);
}
