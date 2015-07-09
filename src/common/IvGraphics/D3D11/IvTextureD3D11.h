//===============================================================================
// @ IvTextureD3D11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
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
    virtual void* BeginLoadData(unsigned int level = 0);
    virtual bool  EndLoadData(unsigned int level = 0);

    virtual void SetAddressingU(IvTextureAddrMode mode);
    virtual void SetAddressingV(IvTextureAddrMode mode);

    virtual void SetMagFiltering(IvTextureMagFilter filter);
    virtual void SetMinFiltering(IvTextureMinFilter filter);

protected:
    // constructor/destructor
    IvTextureD3D11();
    virtual ~IvTextureD3D11();

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
	bool					   mEnableMip;
	D3D11_FILTER               mFilter;
	D3D11_TEXTURE_ADDRESS_MODE mUAddrMode;
	D3D11_TEXTURE_ADDRESS_MODE mVAddrMode;

	DXGI_FORMAT			       mD3DFormat;
}; 

#endif
