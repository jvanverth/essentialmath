//===============================================================================
// @ IvVertexBuffer.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvVertexBuffer__h__
#define __IvVertexBuffer__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBuffer
{
public:
    inline IvVertexFormat GetVertexFormat() { return mVertexFormat; }
    inline unsigned int GetVertexCount() { return mNumVertices; }

protected:
    // constructor/destructor
    IvVertexBuffer() : mVertexFormat( kCPFormat ), mNumVertices( 0 )
    {
    }
	virtual ~IvVertexBuffer() {}

    IvVertexFormat      mVertexFormat;
    unsigned int        mNumVertices;

private:
    // copy operations (unimplemented so we can't copy)
    IvVertexBuffer(const IvVertexBuffer& other);
	IvVertexBuffer& operator=(const IvVertexBuffer& other);
}; 

#endif
