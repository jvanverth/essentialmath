// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

layout(location = POSITION) in vec4 position;
layout(location = NORMAL) in vec3 inNormal;

out vec3 normal;

uniform mat4 IvModelViewProjectionMatrix;

void main()
{
    gl_Position = IvModelViewProjectionMatrix * position;
    
    normal = inNormal;
}
