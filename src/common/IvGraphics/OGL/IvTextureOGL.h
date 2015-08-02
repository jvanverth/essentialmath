//===============================================================================
// @ IvTextureOGL.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
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
    virtual void* BeginLoadData(unsigned int level = 0);
    virtual bool  EndLoadData(unsigned int level = 0);

    virtual void SetAddressingU(IvTextureAddrMode mode);
    virtual void SetAddressingV(IvTextureAddrMode mode);

    virtual void SetMagFiltering(IvTextureMagFilter filter);
    virtual void SetMinFiltering(IvTextureMinFilter filter);

protected:
    // constructor/destructor
    IvTextureOGL();
    virtual ~IvTextureOGL();

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
