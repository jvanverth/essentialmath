// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

uniform mat4 IvModelViewProjectionMatrix;

layout(location = POSITION) in vec3 position;
layout(location = COLOR) in vec4 inColor;

out vec4 color;

void main()
{
    gl_Position = IvModelViewProjectionMatrix*vec4(position,1.0);
    color = inColor;
}
