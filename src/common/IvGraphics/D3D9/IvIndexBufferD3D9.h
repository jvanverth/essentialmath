//===============================================================================
// @ IvIndexBufferD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvIndexBufferD3D9__h__
#define __IvIndexBufferD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvIndexBuffer.h"
#include <d3d9.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvIndexBufferD3D9 : private IvIndexBuffer
{
public:
    // interface routines
    void* BeginLoadData();
    bool  EndLoadData();

    friend class IvResourceManagerD3D9;
    friend class IvRendererD3D9;
    
private:
    // constructor/destructor
    IvIndexBufferD3D9(); 
	~IvIndexBufferD3D9();

    // creation 
    bool Create( unsigned int numVertices, IDirect3DDevice9* device );
    
    // destruction
    void Destroy();
    
    // activate
    bool MakeActive( IDirect3DDevice9* device );

private:
    // copy operations
    IvIndexBufferD3D9(const IvIndexBufferD3D9& other);
	IvIndexBufferD3D9& operator=(const IvIndexBufferD3D9& other);

	IDirect3DIndexBuffer9* mBufferPtr;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif