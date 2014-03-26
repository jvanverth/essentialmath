//===============================================================================
// @ IvIndexBufferOGL.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvIndexBufferOGL.h"
#include <stdio.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::IvIndexBufferOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvIndexBufferOGL::IvIndexBufferOGL() : IvIndexBuffer()
{
    mBufferID = 0;
}	// End of IvIndexBufferOGL::IvIndexBufferOGL()

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::~IvIndexBufferOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvIndexBufferOGL::~IvIndexBufferOGL()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::Create()
//-------------------------------------------------------------------------------
// Create the platform-specific data
//-------------------------------------------------------------------------------
bool
IvIndexBufferOGL::Create( unsigned int numIndices )
{
    if ( numIndices == 0 || mBufferID != 0 )
        return false;
    
    // create the handle
	glGenBuffers( 1, &mBufferID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );
    
    // allocate the memory    
    (void) glGetError();  // clear any previous errors (probably not safe)
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*GL_UNSIGNED_SHORT, 0, GL_STATIC_DRAW );
    if ( glGetError() != GL_NO_ERROR )
    {
        Destroy();
        return false;
    }
    
    mNumIndices = numIndices;
    
    return true;
    
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::Destroy()
//-------------------------------------------------------------------------------
// Destroy the platform-specific data
//-------------------------------------------------------------------------------
void
IvIndexBufferOGL::Destroy()
{
    glDeleteBuffers( 1, &mBufferID );
    mBufferID = 0;
    
    mNumIndices = 0;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active index buffer
//-------------------------------------------------------------------------------
bool
IvIndexBufferOGL::MakeActive()
{
    if ( mBufferID == 0 || mNumIndices == 0 )
        return false;
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::BeginLoadData()
//-------------------------------------------------------------------------------
// Lock down the buffer and start loading
// Returns pointer to client side data area
//-------------------------------------------------------------------------------
void *
IvIndexBufferOGL::BeginLoadData()
{
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );
    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::EndLoadData()
//-------------------------------------------------------------------------------
// Unlock the buffer, we're done loading
// Returns true if all went well
//-------------------------------------------------------------------------------
bool
IvIndexBufferOGL::EndLoadData()
{
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );

    bool ret = (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) != GL_FALSE);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    return ret;
}
