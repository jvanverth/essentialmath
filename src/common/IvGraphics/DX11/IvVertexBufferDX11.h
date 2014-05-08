//===============================================================================
// @ IvVertexBufferDX11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexBufferDX11__h__
#define __IvVertexBufferDX11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexBuffer.h"
#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBufferDX11 : private IvVertexBuffer
{
public:
    // interface routines

    friend class IvResourceManagerDX11;
    friend class IvRendererDX11;
    
private:
    // constructor/destructor
    IvVertexBufferDX11();
	~IvVertexBufferDX11();
    
    // creation 
	bool Create(IvVertexFormat format, unsigned int numVertices, void* data, ID3D11Device* device);
    
    // destruction
    void Destroy();

    // activate
	bool MakeActive(ID3D11DeviceContext* device);

private:
    // copy operations
    IvVertexBufferDX11(const IvVertexBufferDX11& other);
	IvVertexBufferDX11& operator=(const IvVertexBufferDX11& other);

	ID3D11Buffer* mBufferPtr;
	void*		  mDataPtr;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif