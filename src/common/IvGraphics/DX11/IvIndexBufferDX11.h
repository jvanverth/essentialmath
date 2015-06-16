//===============================================================================
// @ IvIndexBufferDX11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvIndexBufferDX11__h__
#define __IvIndexBufferDX11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvIndexBuffer.h"
#include "IvResourceManager.h"

#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvIndexBufferDX11 : private IvIndexBuffer
{
public:
    // interface routines
	virtual void* BeginLoadData();
	virtual bool  EndLoadData();

    friend class IvResourceManagerDX11;
    friend class IvRendererDX11;
    
private:
    // constructor/destructor
    IvIndexBufferDX11(); 
	~IvIndexBufferDX11();

    // creation 
	bool Create(unsigned int numVertices, void* data, IvDataUsage usage, ID3D11Device* device);
    
    // destruction
    void Destroy();
    
    // activate
	bool MakeActive(ID3D11DeviceContext* device);

private:
    // copy operations
    IvIndexBufferDX11(const IvIndexBufferDX11& other);
	IvIndexBufferDX11& operator=(const IvIndexBufferDX11& other);

	ID3D11Buffer* mBufferPtr;
    void*         mDataPtr;
	IvDataUsage   mUsage;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif