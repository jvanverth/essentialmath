//===============================================================================
// @ IvConstantTableD3D11.h
// 
// Replacement for D3D9 constant table
// ------------------------------------------------------------------------------
// Copyright (C) 2014-2015   James M. Van Verth & Lars M. Bishop
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvConstantTableD3D11__h__
#define __IvConstantTableD3D11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include <d3d11.h>
#include <map>

#include "IvUniform.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

typedef int IvConstantHandle;

struct IvConstantDesc
{
    IvUniformType mType;
    union
    {
        struct
        {
            void*         mOffset;
            unsigned int  mCount;
        };
        struct
        {
            intptr_t      mTextureSlot;
            int           mSamplerSlot;
        };
    };
};

class IvConstantTableD3D11
{
public:
    static IvConstantTableD3D11* Create(ID3D11Device* device, ID3DBlob* code);

    void AddRef() { ++mRefCount; }
    void Release() { Destroy(this); }

    bool GetConstantDesc(const char* name, IvConstantDesc* constantDesc);
    void MarkDirty() { mDirty = true;  }

    bool MakeActiveVS(ID3D11DeviceContext* context);
    bool MakeActivePS(ID3D11DeviceContext* context);

private:
    static void Destroy(IvConstantTableD3D11* table);

    // constructor/destructor
    IvConstantTableD3D11() : mRefCount(1), mBuffer(nullptr), mBacking(nullptr),
                            mDirty(false) {}
    ~IvConstantTableD3D11()
    {
        if (mBuffer)
        {
            mBuffer->Release();
            mBuffer = nullptr;
        }
        delete mBacking;
    }

private:
    // copy operations
    IvConstantTableD3D11(const IvConstantTableD3D11& other);
    IvConstantTableD3D11& operator=(const IvConstantTableD3D11& other);

    int                                   mRefCount;
    std::map<std::string, IvConstantDesc> mConstants;
    ID3D11Buffer*                         mBuffer;
    char*                                 mBacking;
    size_t                                mBackingSize;
    bool                                  mDirty;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif