//===============================================================================
// @ IvVertexBufferD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexBufferD3D9__h__
#define __IvVertexBufferD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexBuffer.h"
#include <d3d9.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBufferD3D9 : private IvVertexBuffer
{
public:
    // interface routines
    void* BeginLoadData();
    bool  EndLoadData();

    friend class IvResourceManagerD3D9;
    friend class IvRendererD3D9;
    
private:
    // constructor/destructor
    IvVertexBufferD3D9();
	~IvVertexBufferD3D9();
    
    // creation 
    bool Create( IvVertexFormat format, unsigned int numVertices, IDirect3DDevice9* device );
    
    // destruction
    void Destroy();

    // activate
    bool MakeActive( IDirect3DDevice9* device );

private:
    // copy operations
    IvVertexBufferD3D9(const IvVertexBufferD3D9& other);
	IvVertexBufferD3D9& operator=(const IvVertexBufferD3D9& other);

	IDirect3DVertexBuffer9* mBufferPtr;
	void*					mDataPtr;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif