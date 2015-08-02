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

void main()
{
    fragColor = texture(Texture, uv) * scale;
    
    float maxVal = max(fragColor.r, fragColor.g);
    maxVal = max(maxVal, fragColor.b);
    maxVal = max(maxVal, 1.0);
    
    fragColor /= maxVal;
}
