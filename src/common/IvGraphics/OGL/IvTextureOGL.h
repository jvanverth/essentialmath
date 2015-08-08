//===============================================================================
// @ IvTextureOGL.h
// 
// OpenGL implementation for texture
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvTextureOGL__h__
#define __IvTextureOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvTexture.h"
#include "IvResourceManager.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvTextureOGL: public IvTexture
{
public:
    friend class IvResourceManagerOGL;
    friend class IvUniformOGL;

    // interface routines
    void* BeginLoadData(unsigned int level = 0) final;
    bool  EndLoadData(unsigned int level = 0) final;

    void SetAddressingU(IvTextureAddrMode mode) final;
    void SetAddressingV(IvTextureAddrMode mode) final;

    void SetMagFiltering(IvTextureMagFilter filter) final;
    void SetMinFiltering(IvTextureMinFilter filter) final;

protected:
    // constructor/destructor
    IvTextureOGL();
    ~IvTextureOGL() final;

    // creation
    bool Create(unsigned int width, unsigned int height, IvTextureFormat format,
                void* data, IvDataUsage usage);
    bool CreateMipmapped(unsigned int width, unsigned int height, IvTextureFormat format,
                void** data, unsigned int levels, IvDataUsage usage);

    // destruction
    void Destroy();

    // activation
    void MakeActive(unsigned int unit = 0);

    unsigned int mID;
    unsigned int mLevelCount;
    
    IvDataUsage  mUsage;
    
    // used for BeginLoadData/EndLoadData
    unsigned char* mTempData;

private:
    // copy operations (unimplemented so we can't copy)
    IvTextureOGL(const IvTextureOGL& other);
    IvTextureOGL& operator=(const IvTextureOGL& other);
}; 

#endif
