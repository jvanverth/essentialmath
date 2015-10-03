// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#version 150

uniform sampler2D Texture;
uniform float scale;

in vec2 uv;
in vec4 color;

out vec4 fragColor;

// uses John Hable's filmic curve from Uncharted 2
// See http://www.gdcvault.com/play/1012459/Uncharted_2__HDR_Lighting
// and http://filmicgames.com/archives/75

float A = 0.22;  // Shoulder strength
float B = 0.30;  // Linear strength
float C = 0.10;  // Linear angle
float D = 0.20;  // Toe strength
float E = 0.01;  // Toe numerator
float F = 0.30;  // Toe denominator
float W = 11.2;  // Base white value

vec3 Uncharted2Tonemap(vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
    vec4 texColor = texture(Texture, uv) * scale;

    float ExposureBias = 2.0;
    vec3 corrected = Uncharted2Tonemap(ExposureBias*texColor.rgb);

    vec3 whiteScale = vec3(1.0/Uncharted2Tonemap(vec3(scale*11.2/16.0)));

    fragColor = vec4(corrected*whiteScale, texColor.a);
}
