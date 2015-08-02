//===============================================================================
// @ IvVertexBufferD3D11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexBufferD3D11__h__
#define __IvVertexBufferD3D11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexBuffer.h"
#include "IvResourceManager.h"
#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBufferD3D11 : private IvVertexBuffer
{
public:
    // interface routines
    virtual void* BeginLoadData();
    virtual bool  EndLoadData();

    friend class IvResourceManagerD3D11;
    friend class IvRendererD3D11;
    
private:
    // constructor/destructor
    IvVertexBufferD3D11();
    ~IvVertexBufferD3D11();
    
    // creation 
    bool Create(IvVertexFormat format, unsigned int numVertices, void* data, IvDataUsage usage,
                ID3D11Device* device);
    
    // destruction
    void Destroy();

    // activate
    bool MakeActive(ID3D11DeviceContext* device);

private:
    // copy operations
    IvVertexBufferD3D11(const IvVertexBufferD3D11& other);
    IvVertexBufferD3D11& operator=(const IvVertexBufferD3D11& other);

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