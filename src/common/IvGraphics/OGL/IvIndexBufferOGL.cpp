//===============================================================================
// @ IvIndexBufferOGL.cpp
// 
// OpenGL implementation for index buffer implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvIndexBufferOGL.h"
#include "IvAssert.h"
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
    , mBufferID(0)
{
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::~IvIndexBufferOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvIndexBufferOGL::~IvIndexBufferOGL()
{
    ASSERT(!mBufferID);
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferOGL::Create()
//-------------------------------------------------------------------------------
// Create the platform-specific data
//-------------------------------------------------------------------------------
bool
IvIndexBufferOGL::Create( unsigned int numIndices, void* data, IvDataUsage usage )
{
    if ( numIndices == 0 || mBufferID != 0 )
    {
        return false;
    }
    
    if ( usage == kImmutableUsage && !data )
    {
        return false;
    }
    
    // create the handle
    glGenBuffers( 1, &mBufferID );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );
    
    // allocate the memory    
    (void) glGetError();  // clear any previous errors (probably not safe)
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GL_UNSIGNED_INT), data,
                  usage == kDynamicUsage ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
    if ( glGetError() != GL_NO_ERROR )
    {
        Destroy();
        return false;
    }
    
    mNumIndices = numIndices;
    
    mUsage = usage;
    
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
    if (mUsage == kImmutableUsage)
    {
        return nullptr;
    }
    
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
    if (mUsage == kImmutableUsage)
    {
        return false;
    }
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferID );
    bool ret = (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) != GL_FALSE);

    return ret;
}
