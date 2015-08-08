//===============================================================================
// @ IvVertexBufferD3D11.h
// 
// D3D11 vertex buffer implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015   James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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
    void* BeginLoadData() final;
    bool  EndLoadData() final;

    friend class IvResourceManagerD3D11;
    friend class IvRendererD3D11;

    ID3D11Buffer* GetBufferPtr() { return mBufferPtr; }

private:
    // constructor/destructor
    IvVertexBufferD3D11();
    ~IvVertexBufferD3D11() final;
    
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