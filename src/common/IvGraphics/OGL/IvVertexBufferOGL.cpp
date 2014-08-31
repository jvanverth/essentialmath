//===============================================================================
// @ IvVertexBufferOGL.cpp
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

#include "IvVertexBufferOGL.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::IvVertexBufferOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvVertexBufferOGL::IvVertexBufferOGL() : IvVertexBuffer(), mBufferID(0)
{
}	// End of IvVertexBufferOGL::IvVertexBufferOGL()

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::~IvVertexBufferOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvVertexBufferOGL::~IvVertexBufferOGL()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::Create()
//-------------------------------------------------------------------------------
// Create the resources for the vertex buffer
//-------------------------------------------------------------------------------
bool
IvVertexBufferOGL::Create( IvVertexFormat format, unsigned int numVertices, void* data, bool dynamic )
{
    if ( numVertices == 0 || mBufferID != 0 )
        return false;
    
    // create the handle
    glGenBuffers( 1, &mBufferID );
    glBindBuffer( GL_ARRAY_BUFFER, mBufferID );
    
    // allocate the memory
    (void) glGetError();  // clear any previous errors (probably not safe)
    glBufferData( GL_ARRAY_BUFFER, numVertices*kIvVFSize[format], data,
                 dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
    if ( glGetError() != GL_NO_ERROR )
    {
        Destroy();
        return false;
    }
    
    mVertexFormat = format;
    mNumVertices = numVertices;

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::Destroy()
//-------------------------------------------------------------------------------
// Destroy the resources for the vertex buffer
//-------------------------------------------------------------------------------
void
IvVertexBufferOGL::Destroy()
{
    // clear the handle and any associated memory
    glDeleteBuffers( 1, &mBufferID );
    mBufferID = 0;
    
    mNumVertices = 0;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active vertex buffer
//-------------------------------------------------------------------------------
bool
IvVertexBufferOGL::MakeActive()
{
    if ( mBufferID == 0 || mNumVertices == 0 )
        return false;
    
    glBindBuffer( GL_ARRAY_BUFFER, mBufferID );
    
    // set arrays active
    switch (mVertexFormat)
    {
        case kCPFormat:
        default:
            glInterleavedArrays( GL_C4UB_V3F, 0, NULL );
            break;
            
        case kNPFormat:
            glInterleavedArrays( GL_N3F_V3F, 0, NULL );
            break;
            
		case kCNPFormat:
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(IvCNPVertex), BUFFER_OFFSET( 0 ));
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, sizeof(IvCNPVertex), BUFFER_OFFSET( 4 ));
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(IvCNPVertex), BUFFER_OFFSET( 16 ));	
			break;

        case kTCPFormat:
            glInterleavedArrays( GL_T2F_C4UB_V3F, 0, NULL );
            break;
            
        case kTNPFormat:
            glInterleavedArrays( GL_T2F_N3F_V3F, 0, NULL );
            break;
    }
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::BeginLoadData()
//-------------------------------------------------------------------------------
// Lock down the buffer and start loading
// Returns pointer to client side data area
//-------------------------------------------------------------------------------
void *
IvVertexBufferOGL::BeginLoadData()
{
    glBindBuffer( GL_ARRAY_BUFFER, mBufferID );
    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferOGL::EndLoadData()
//-------------------------------------------------------------------------------
// Unlock the buffer, we're done loading
// Returns true if all went well
//-------------------------------------------------------------------------------
bool
IvVertexBufferOGL::EndLoadData()
{
    glBindBuffer( GL_ARRAY_BUFFER, mBufferID );
    bool ret = glUnmapBuffer(GL_ARRAY_BUFFER) != GL_FALSE;
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    return ret;
}

