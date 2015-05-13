//===============================================================================
// @ IvTextureDX11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvTextureDX11__h__
#define __IvTextureDX11__h__

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

class IvTextureDX11: public IvTexture
{
public:
    friend class IvResourceManagerDX11;
    friend class IvUniformDX11;

    // interface routines
    virtual void* BeginLoadData(unsigned int level = 0);
    virtual bool  EndLoadData(unsigned int level = 0);

    virtual void SetAddressingU(IvTextureAddrMode mode);
    virtual void SetAddressingV(IvTextureAddrMode mode);

    virtual void SetMagFiltering(IvTextureMagFilter filter);
    virtual void SetMinFiltering(IvTextureMinFilter filter);

protected:
    // constructor/destructor
    IvTextureDX11();
    virtual ~IvTextureDX11();

    // creation
	bool Create(unsigned int width, unsigned int height, IvTextureFormat format,
		        void* data, IvDataUsage usage, ID3D11Device* device);
	bool CreateMipmapped(unsigned int width, unsigned int height, IvTextureFormat format,
		        void** data, unsigned int levels, IvDataUsage usage, ID3D11Device* device);

	// destruction
	void Destroy();

	// activation
	void MakeActive(unsigned int unit, ID3D11Device* device);

    unsigned int mLevelCount;

	IvDataUsage  mUsage;

private:
    // copy operations (unimplemented so we can't copy)
    IvTextureDX11(const IvTextureDX11& other);
	IvTextureDX11& operator=(const IvTextureDX11& other);

	// D3D objects
	ID3D11Texture2D*          mTexturePtr;
	ID3D11ShaderResourceView* mShaderResourceView;

	unsigned int		mUAddrMode;
	unsigned int		mVAddrMode;
	unsigned int		mMagFilter;
	unsigned int		mMinFilter;
	unsigned int		mMipFilter;

	DXGI_FORMAT			mD3DFormat;
}; 

#endif
