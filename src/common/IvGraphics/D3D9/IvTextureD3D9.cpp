//===============================================================================
// @ IvTextureD3D9.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#include "IvTextureD3D9.h"
#include "IvAssert.h"
#include <d3dx9tex.h>

static unsigned int sTextureFormatSize[kTexFmtCount] = {4, 3};

static D3DFORMAT	sD3DTextureFormat[kTexFmtCount] = {D3DFMT_A8R8G8B8, D3DFMT_X8B8G8R8};

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::IvTextureD3D9()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvTextureD3D9::IvTextureD3D9() : IvTexture()
	, mTexturePtr(0)
{
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::~IvTextureD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvTextureD3D9::~IvTextureD3D9()
{
	Destroy();
}


//-------------------------------------------------------------------------------
// @ IvTextureD3D9::Create()
//-------------------------------------------------------------------------------
// Texture initialization
//-------------------------------------------------------------------------------
bool
IvTextureD3D9::Create(unsigned int width, unsigned int height, IvTextureFormat format, IDirect3DDevice9* device)
{
    mWidth = width;
    mHeight = height;
    mFormat = format;

    unsigned int texelSize = sTextureFormatSize[mFormat];

    mLevelCount = 1;

    int size = texelSize * width * height;
    while ((width != 1) || (height != 1))
    {
        width >>= 1;
        height >>= 1;

        if (!width)
            width = 1;

        if (!height)
            height = 1;

        size += texelSize * width * height;
        mLevelCount++;
    }

    mLevels = new Level[mLevelCount];

    mLevels[0].mData = 0;
    mLevels[0].mWidth = mWidth;
    mLevels[0].mHeight = mHeight;
    mLevels[0].mSize = mWidth * mHeight * texelSize;

    width = mWidth;
    height = mHeight;

    unsigned int i;
    for (i = 1; i < mLevelCount; i++)
    {
        width >>= 1;
        height >>= 1;

        if (!width)
            width = 1;

        if (!height)
            height = 1;

        unsigned int size = texelSize * width * height;

        mLevels[i].mData = 0;
        mLevels[i].mWidth = width;
        mLevels[i].mHeight = height;
        mLevels[i].mSize = size;
    }

	if (FAILED(D3DXCreateTexture(device, mWidth, mHeight, mLevelCount, D3DUSAGE_AUTOGENMIPMAP, sD3DTextureFormat[format], 
		                             D3DPOOL_MANAGED, &mTexturePtr)))
	{
		return false;
	}

	D3DSURFACE_DESC desc;
	mTexturePtr->GetLevelDesc(0, &desc);
	mD3DFormat = desc.Format;

	// verify formats

	return true;
}


//-------------------------------------------------------------------------------
// @ IvTextureD3D9::Destroy()
//-------------------------------------------------------------------------------
// Destroy!
//-------------------------------------------------------------------------------
void
IvTextureD3D9::Destroy()
{
	if ( mTexturePtr )
	{
		mTexturePtr->Release();
		mTexturePtr = 0;
	}
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::MakeActive()
//-------------------------------------------------------------------------------
// Binds the texture to the desired unit
//-------------------------------------------------------------------------------
void IvTextureD3D9::MakeActive(unsigned int unit, IDirect3DDevice9* device)
{
	device->SetSamplerState(unit, D3DSAMP_ADDRESSU, mUAddrMode);
	device->SetSamplerState(unit, D3DSAMP_ADDRESSV, mVAddrMode);
	device->SetSamplerState(unit, D3DSAMP_MAGFILTER, mMagFilter);
	device->SetSamplerState(unit, D3DSAMP_MINFILTER, mMinFilter);
	device->SetSamplerState(unit, D3DSAMP_MIPFILTER, mMipFilter);

	device->SetTexture(unit, mTexturePtr);
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::BeginLoadData()
//-------------------------------------------------------------------------------
// "Locks" and returns a pointer to the sysmem texture data for the given level
//-------------------------------------------------------------------------------
void* IvTextureD3D9::BeginLoadData(unsigned int level)
{
	mLevels[level].mData = (void*) new unsigned char[mLevels[level].mSize];
	return mLevels[level].mData;
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::EndLoadData()
//-------------------------------------------------------------------------------
// Uploads the edited texture data to D3D
//-------------------------------------------------------------------------------
bool  IvTextureD3D9::EndLoadData(unsigned int level)
{
	D3DLOCKED_RECT lockedRect;
    if (FAILED(mTexturePtr->LockRect(level, &lockedRect, 0, 0)))
	{
		delete mLevels[level].mData;
		return false;
	}

	unsigned int height = mLevels[level].mHeight;
	unsigned int width = mLevels[level].mWidth;
	unsigned int srcPitch = sTextureFormatSize[mFormat]*width;
	unsigned int dstPitch = (unsigned int) lockedRect.Pitch;

	unsigned char* srcBits = static_cast<unsigned char*>(mLevels[level].mData);
	unsigned int srcBytes = sTextureFormatSize[mFormat];
	unsigned int srcRedIndex = 0;
	unsigned int srcGreenIndex = 1;
	unsigned int srcBlueIndex = 2;
	unsigned int srcAlphaIndex = 3;

	unsigned char* dstBits = static_cast<unsigned char*>(lockedRect.pBits);
	unsigned int dstBytes;
	unsigned int dstRedIndex;
	unsigned int dstGreenIndex;
	unsigned int dstBlueIndex;
	unsigned int dstAlphaIndex = 0;
	switch (mD3DFormat)
	{ 
	default:
		ASSERT( false );
		break;
	case D3DFMT_A8R8G8B8:
		dstBytes = 4;
		dstRedIndex = 2;
		dstGreenIndex = 1;
		dstBlueIndex = 0;
		dstAlphaIndex = 3;
		break;
	case D3DFMT_X8R8G8B8:
		dstBytes = 4;
		dstRedIndex = 2;
		dstGreenIndex = 1;
		dstBlueIndex = 0;
		dstAlphaIndex = 3;
		break;
	case D3DFMT_R8G8B8:
		dstBytes = 3;
		dstRedIndex = 2;
		dstGreenIndex = 1;
		dstBlueIndex = 0;
		break;
	}

	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			dstBits[dstBytes*j + dstRedIndex + i*dstPitch] = srcBits[srcBytes*j + srcRedIndex + i*srcPitch];
			dstBits[dstBytes*j + dstGreenIndex + i*dstPitch] = srcBits[srcBytes*j + srcGreenIndex + i*srcPitch];
			dstBits[dstBytes*j + dstBlueIndex + i*dstPitch] = srcBits[srcBytes*j + srcBlueIndex + i*srcPitch];
			if ( srcBytes == 4 )
				dstBits[dstBytes*j + dstAlphaIndex + i*dstPitch] = srcBits[srcBytes*j + srcAlphaIndex + i*srcPitch];
		}
	}

	delete mLevels[level].mData;

	return (D3D_OK == mTexturePtr->UnlockRect(level));
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::SetAddressingU()
//-------------------------------------------------------------------------------
// Sets the texture wrapping in U
//-------------------------------------------------------------------------------
void IvTextureD3D9::SetAddressingU(IvTextureAddrMode mode)
{
	mUAddrMode = (mode == kClampTexAddr) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::SetAddressingV()
//-------------------------------------------------------------------------------
// Sets the texture wrapping in V
//-------------------------------------------------------------------------------
void IvTextureD3D9::SetAddressingV(IvTextureAddrMode mode)
{
	mVAddrMode = (mode == kClampTexAddr) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::SetMagFiltering()
//-------------------------------------------------------------------------------
// Sets the texture magnification filter
//-------------------------------------------------------------------------------
void IvTextureD3D9::SetMagFiltering(IvTextureMagFilter filter)
{
	mMagFilter = (filter == kNearestTexMagFilter) ? D3DTEXF_POINT : D3DTEXF_LINEAR;
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::SetMinFiltering()
//-------------------------------------------------------------------------------
// Sets the texture minification filter
//-------------------------------------------------------------------------------
void IvTextureD3D9::SetMinFiltering(IvTextureMinFilter filter)
{
    switch(filter)
    {
        case kNearestTexMinFilter:
			mMinFilter = D3DTEXF_POINT;
            mMipFilter = D3DTEXF_NONE;
            break;
        case kBilerpTexMinFilter:
			mMinFilter = D3DTEXF_LINEAR;
            mMipFilter = D3DTEXF_NONE;
            break;
        case kNearestMipmapNearestTexMinFilter:
			mMinFilter = D3DTEXF_POINT;
            mMipFilter = D3DTEXF_POINT;
            break;
        case kBilerpMipmapNearestTexMinFilter:
			mMinFilter = D3DTEXF_POINT;
            mMipFilter = D3DTEXF_LINEAR;
            break;
        case kNearestMipmapLerpTexMinFilter:
			mMinFilter = D3DTEXF_LINEAR;
            mMipFilter = D3DTEXF_POINT;
            break;
        case kBilerpMipmapLerpTexMinFilter:
			mMinFilter = D3DTEXF_LINEAR;
            mMipFilter = D3DTEXF_LINEAR;
            break;
    };
}

//-------------------------------------------------------------------------------
// @ IvTextureD3D9::GenerateMipmapPyramid()
//-------------------------------------------------------------------------------
// Fills the mipmap levels based on the finest level (0)
//-------------------------------------------------------------------------------
void IvTextureD3D9::GenerateMipmapPyramid()
{
    if (mTexturePtr)
	{
		mTexturePtr->SetAutoGenFilterType(D3DTEXF_GAUSSIANQUAD);
		mTexturePtr->GenerateMipSubLevels();
	}
}

