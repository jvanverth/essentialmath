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

    virtual void GenerateMipmapPyramid();

protected:
    // constructor/destructor
    IvTextureOGL();
    virtual ~IvTextureOGL();

    // creation
    bool Create(unsigned int width, unsigned int height, IvTextureFormat format);

	// destruction
    void Destroy();

	// activation
    void MakeActive(unsigned int unit = 0);

    unsigned int mID;

    struct Level
    {
        void* mData;
        unsigned int mSize;
        unsigned int mWidth;
        unsigned int mHeight;
    };

    Level* mLevels;
    unsigned int mLevelCount;

private:
    // copy operations (unimplemented so we can't copy)
    IvTextureOGL(const IvTextureOGL& other);
	IvTextureOGL& operator=(const IvTextureOGL& other);
}; 

#endif
