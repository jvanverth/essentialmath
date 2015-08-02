//===============================================================================
// @ IvVertexShaderOGL.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015   James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexShaderOGL__h__
#define __IvVertexShaderOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexShader.h"
#include "../IvVertexFormats.h"
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvResourceManagerOGL;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexShaderOGL : private IvVertexShader
{
public:
    // interface routines

    friend class IvResourceManagerOGL;
    
private:
    // constructor/destructor
    IvVertexShaderOGL();
    ~IvVertexShaderOGL();
    
    // initialization
    bool CreateFromFile( const char* filename );
    bool CreateFromString( const char* string );
    bool CreateDefault( IvVertexFormat format );

    void Destroy();
    
private:
    // copy operations
    IvVertexShaderOGL(const IvVertexShaderOGL& other);
    IvVertexShaderOGL& operator=(const IvVertexShaderOGL& other);

    GLuint mShaderID;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
