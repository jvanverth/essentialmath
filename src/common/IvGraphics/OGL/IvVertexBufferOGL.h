//===============================================================================
// @ IvVertexBufferOGL.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexBufferOGL__h__
#define __IvVertexBufferOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexBuffer.h"
#ifdef PLATFORM_OSX
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
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
    friend class IvResourceManagerOGL;
    friend class IvRendererOGL;
    
private:
    // constructor/destructor
    IvVertexBufferOGL();
	~IvVertexBufferOGL();
    
    // creation 
    bool Create( IvVertexFormat format, unsigned int numVertices, void* data );
    
    // destruction
    void Destroy();

    // activate
    bool MakeActive();

private:
    // copy operations
    IvVertexBufferOGL(const IvVertexBufferOGL& other);
	IvVertexBufferOGL& operator=(const IvVertexBufferOGL& other);

	GLuint mBufferID;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
