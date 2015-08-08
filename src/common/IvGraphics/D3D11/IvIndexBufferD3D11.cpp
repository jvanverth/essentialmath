//===============================================================================
// @ IvIndexBufferD3D11.cpp
// 
// D3D11 index buffer implementation
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

#include "IvIndexBufferD3D11.h"

#include "IvAssert.h"
#include "IvRendererD3D11.h"
#include "IvTypes.h"
#include <stdio.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::IvIndexBufferD3D11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvIndexBufferD3D11::IvIndexBufferD3D11() : IvIndexBuffer(), mBufferPtr(0), mDataPtr(0)
{
}   // End of IvIndexBufferD3D11::IvIndexBufferD3D11()

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::~IvIndexBufferD3D11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvIndexBufferD3D11::~IvIndexBufferD3D11()
{
    ASSERT(!mBufferPtr);
    ASSERT(!mDataPtr);
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::Create()
//-------------------------------------------------------------------------------
// Create the platform-specific data
//-------------------------------------------------------------------------------
bool
IvIndexBufferD3D11::Create(unsigned int numIndices, void* data, IvDataUsage usage, ID3D11Device* device)
{
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.ByteWidth = sizeof(unsigned int)*numIndices;
    switch (usage)
    {
    default:
    case kDefaultUsage:
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        break;
    case kDynamicUsage:
        indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        break;
    case kImmutableUsage:
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        break;
    }
    mUsage = usage;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = (usage == kDynamicUsage) ? D3D11_CPU_ACCESS_WRITE : 0;
    indexBufferDesc.MiscFlags = 0;
    //*** replacement for D3DMANAGED?

    D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
    D3D11_SUBRESOURCE_DATA initData;
    if (data)
    {
        initDataPtr = &initData;
        initData.pSysMem = data;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;
    }

    if (FAILED(device->CreateBuffer(&indexBufferDesc, initDataPtr, &mBufferPtr)))
    {
        mBufferPtr = 0;
        return false;
    }

    mNumIndices = numIndices;

    return true;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::Destroy()
//-------------------------------------------------------------------------------
// Destroy the platform-specific data
//-------------------------------------------------------------------------------
void
IvIndexBufferD3D11::Destroy()
{
    delete [] mDataPtr;
    mDataPtr = 0;

    if (mBufferPtr)
    {
        // clear the handle and any associated memory
        mBufferPtr->Release();
        mBufferPtr = 0;
    }
    
    mNumIndices = 0;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active index buffer
//-------------------------------------------------------------------------------
bool
IvIndexBufferD3D11::MakeActive(ID3D11DeviceContext* context)
{
    if ( mBufferPtr == 0 || mNumIndices == 0 )
        return false;
    
    context->IASetIndexBuffer(mBufferPtr, DXGI_FORMAT_R32_UINT, 0);
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::BeginLoadData()
//-------------------------------------------------------------------------------
// Lock down the buffer and start loading
// Returns pointer to client side data area
//-------------------------------------------------------------------------------
void *
IvIndexBufferD3D11::BeginLoadData()
{
    if (mUsage == kImmutableUsage)
    {
        return nullptr;
    }

    mDataPtr = (void*) new unsigned char[mNumIndices*sizeof(unsigned int)];

    return mDataPtr;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D11::EndLoadData()
//-------------------------------------------------------------------------------
// Unlock the buffer, we're done loading
// Returns true if all went well
//-------------------------------------------------------------------------------
bool
IvIndexBufferD3D11::EndLoadData()
{
    if (mUsage == kImmutableUsage)
    {
        return false;
    }

    // not already "locked"
    if (!mDataPtr)
    {
        return false;
    }

    // this is seriously ugly -- not clear how to easily get the context down here
    ID3D11DeviceContext* d3dContext = ((IvRendererD3D11*)IvRenderer::mRenderer)->GetContext();
    if (kDefaultUsage == mUsage)
    {
        // use UpdateSubresource()
        d3dContext->UpdateSubresource(mBufferPtr, 0, nullptr, mDataPtr, mNumIndices*sizeof(unsigned int), mNumIndices*sizeof(unsigned int));
    }
    else if (kDynamicUsage == mUsage)
    {
        // use Map/Unmap
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        //  Disable GPU access to the vertex buffer data.
        if (S_OK != d3dContext->Map(mBufferPtr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
        {
            return false;
        }

        void* bufferData = mappedResource.pData;
        memcpy(bufferData, mDataPtr, mNumIndices*sizeof(unsigned int));

        //  Reenable GPU access to the vertex buffer data.
        d3dContext->Unmap(mBufferPtr, 0);
    }

    delete[] mDataPtr;
    mDataPtr = nullptr;

    return true;
}

