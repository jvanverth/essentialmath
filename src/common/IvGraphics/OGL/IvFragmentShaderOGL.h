//===============================================================================
// @ IvFragmentShaderOGL.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
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
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
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
