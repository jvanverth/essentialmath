//===============================================================================
// @ IvVertexBufferOGL.h
// 
// OpenGL vertex buffer implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015   James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvVertexBufferOGL__h__
#define __IvVertexBufferOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexBuffer.h"
#include "IvResourceManager.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBufferOGL : private IvVertexBuffer
{
public:
    // interface routines
    virtual void* BeginLoadData();
    virtual bool  EndLoadData();
    
    friend class IvResourceManagerOGL;
    friend class IvRendererOGL;
    
private:
    // constructor/destructor
    IvVertexBufferOGL();
    ~IvVertexBufferOGL();
    
    // creation 
    bool Create( IvVertexFormat format, unsigned int numVertices, void* data,
                 IvDataUsage usage );
    
    // destruction
    void Destroy();

    // activate
    bool MakeActive();

private:
    // copy operations
    IvVertexBufferOGL(const IvVertexBufferOGL& other);
    IvVertexBufferOGL& operator=(const IvVertexBufferOGL& other);

    GLuint      mBufferID;
    GLuint      mVertexArrayID;
    IvDataUsage mUsage;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
