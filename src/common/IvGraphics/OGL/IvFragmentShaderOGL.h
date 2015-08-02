//===============================================================================
// @ IvFragmentShaderOGL.h
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

#ifndef __IvFragmentShaderOGL__h__
#define __IvFragmentShaderOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvFragmentShader.h"
#include "../IvVertexFormats.h"
#ifdef PLATFORM_OSX
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

class IvFragmentShaderOGL : private IvFragmentShader
{
public:
    // interface routines

    friend class IvResourceManagerOGL;
    
private:
    // constructor/destructor
    IvFragmentShaderOGL();
    ~IvFragmentShaderOGL();
    
    // initialization
    bool CreateFromFile( const char* filename );
    bool CreateFromString( const char* filename );
    bool CreateDefault( IvVertexFormat format );
    
    void Destroy();
    
private:
    // copy operations
    IvFragmentShaderOGL(const IvFragmentShaderOGL& other);
    IvFragmentShaderOGL& operator=(const IvFragmentShaderOGL& other);

    GLuint mShaderID;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
