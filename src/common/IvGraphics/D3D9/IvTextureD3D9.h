//===============================================================================
// @ IvTextureD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvTextureD3D9__h__
#define __IvTextureD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvTexture.h"
#include <d3d9.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvTextureD3D9: public IvTexture
{
public:
    friend class IvResourceManagerD3D9;
    friend class IvUniformD3D9;

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
    IvTextureD3D9();
    virtual ~IvTextureD3D9();

    // creation
	bool Create(unsigned int width, unsigned int height, IvTextureFormat format, IDirect3DDevice9* device); 

	// destruction
	void Destroy();

	// activation
    void MakeActive(unsigned int unit, IDirect3DDevice9* device);

    unsigned int mLevelCount;

    struct Level
    {
        void* mData;
        unsigned int mSize;
        unsigned int mWidth;
        unsigned int mHeight;
    };

    Level* mLevels;

private:
    // copy operations (unimplemented so we can't copy)
    IvTextureD3D9(const IvTextureD3D9& other);
	IvTextureD3D9& operator=(const IvTextureD3D9& other);

	// D3D objects
	IDirect3DTexture9* mTexturePtr;

	unsigned int		mUAddrMode;
	unsigned int		mVAddrMode;
	unsigned int		mMagFilter;
	unsigned int		mMinFilter;
	unsigned int		mMipFilter;

	_D3DFORMAT			mD3DFormat;

}; 

#endif
