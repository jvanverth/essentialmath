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
#include <string.h>

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
IvIndexBufferOGL::Create( unsigned int numIndices, void* data )
{
    if ( numIndices == 0 || mBufferID != 0 )
        return false;
    
    // create the handle
	glGenBuffers( 1, &mBufferID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );
    
    // allocate the memory    
    (void) glGetError();  // clear any previous errors (probably not safe)
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GL_UNSIGNED_INT), data, GL_STATIC_DRAW );
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
