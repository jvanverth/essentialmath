//===============================================================================
// @ IvTextureD3D11.h
// 
// D3D11 texture implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvTextureD3D11__h__
#define __IvTextureD3D11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvTexture.h"
#include "IvResourceManager.h"

#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvTextureD3D11: public IvTexture
{
public:
    friend class IvResourceManagerD3D11;
    friend class IvUniformD3D11;

    // interface routines
    void* BeginLoadData(unsigned int level = 0) final;
    bool  EndLoadData(unsigned int level = 0) final;

    void SetAddressingU(IvTextureAddrMode mode) final;
    void SetAddressingV(IvTextureAddrMode mode) final;

    void SetMagFiltering(IvTextureMagFilter filter) final;
    void SetMinFiltering(IvTextureMinFilter filter) final;

protected:
    // constructor/destructor
    IvTextureD3D11();
    ~IvTextureD3D11() final;

    // creation
    bool Create(unsigned int width, unsigned int height, IvTextureFormat format,
                void* data, IvDataUsage usage, ID3D11Device* device);
    bool CreateMipmapped(unsigned int width, unsigned int height, IvTextureFormat format,
                void** data, unsigned int levels, IvDataUsage usage, ID3D11Device* device);

    // destruction
    void Destroy();

    // activation
    void MakeActive(unsigned int textureUnit, unsigned int samplerUnit, ID3D11Device* device);

    unsigned int mLevelCount;

    struct Level
    {
        void* mData;
        size_t mSize;
        unsigned int mWidth;
        unsigned int mHeight;
    };

    Level* mLevels;

    IvDataUsage  mUsage;

private:
    // copy operations (unimplemented so we can't copy)
    IvTextureD3D11(const IvTextureD3D11& other);
    IvTextureD3D11& operator=(const IvTextureD3D11& other);

    // D3D objects
    ID3D11Texture2D*          mTexturePtr;
    ID3D11ShaderResourceView* mShaderResourceView;

    IvTextureMagFilter         mMagFilter;
    IvTextureMinFilter         mMinFilter;
    bool                       mEnableMip;
    D3D11_FILTER               mFilter;
    D3D11_TEXTURE_ADDRESS_MODE mUAddrMode;
    D3D11_TEXTURE_ADDRESS_MODE mVAddrMode;

    DXGI_FORMAT                mD3DFormat;
}; 

#endif
