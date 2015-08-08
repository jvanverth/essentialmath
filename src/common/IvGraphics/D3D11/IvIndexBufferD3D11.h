//===============================================================================
// @ IvIndexBufferD3D11.h
// 
// D3D11 index buffer implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015   James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvIndexBufferD3D11__h__
#define __IvIndexBufferD3D11__h__

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

class IvIndexBufferD3D11 : private IvIndexBuffer
{
public:
    // interface routines
    void* BeginLoadData() final;
    bool  EndLoadData() final;

    friend class IvResourceManagerD3D11;
    friend class IvRendererD3D11;
    
private:
    // constructor/destructor
    IvIndexBufferD3D11(); 
    ~IvIndexBufferD3D11() final;

    // creation 
    bool Create(unsigned int numVertices, void* data, IvDataUsage usage, ID3D11Device* device);
    
    // destruction
    void Destroy();
    
    // activate
    bool MakeActive(ID3D11DeviceContext* device);

private:
    // copy operations
    IvIndexBufferD3D11(const IvIndexBufferD3D11& other);
    IvIndexBufferD3D11& operator=(const IvIndexBufferD3D11& other);

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