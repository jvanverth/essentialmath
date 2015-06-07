//===============================================================================
// @ IvTextureDX11.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#include "IvTextureDX11.h"
#include "IvRendererDX11.h"
#include "IvAssert.h"

// 24-bit formats aren't supported in DX11
// will need to convert before creating
static unsigned int sInternalTextureFormatSize[kTexFmtCount] = {4, 4};
static unsigned int sExternalTextureFormatSize[kTexFmtCount] = {4, 3};
static DXGI_FORMAT	sD3DTextureFormat[kTexFmtCount] = { DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM };

//-------------------------------------------------------------------------------
// @ IvTextureDX11::IvTextureDX11()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvTextureDX11::IvTextureDX11() : IvTexture()
	, mTexturePtr(0)
	, mShaderResourceView(0)
{
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::~IvTextureDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvTextureDX11::~IvTextureDX11()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// @ Convert24Bit()
//-------------------------------------------------------------------------------
// Utility function to convert 24-bit color data to 32-bit
//-------------------------------------------------------------------------------
static void Convert24Bit(void* outData, void* inData, unsigned int width, unsigned int height)
{
	unsigned char* inComponent = (unsigned char*)inData;
	unsigned char* outComponent = (unsigned char*)outData;
	for (unsigned int j = 0; j < height; ++j)
	{
		for (unsigned int i = 0; i < width; ++i)
		{
			// RGB
			*outComponent++ = *inComponent++;
			*outComponent++ = *inComponent++;
			*outComponent++ = *inComponent++;
			// A
			*outComponent++ = 0xff;
		}
	}
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::Create()
//-------------------------------------------------------------------------------
// Texture initialization
//-------------------------------------------------------------------------------
bool
IvTextureDX11::Create(unsigned int width, unsigned int height, IvTextureFormat format, 
                      void* data, IvDataUsage usage, ID3D11Device* device)
{
	if (width == 0 || height == 0 || mTexturePtr)
	{
		return false;
	}

	if (usage == kImmutableUsage && !data)
	{
		return false;
	}

	mWidth = width;
    mHeight = height;
    mFormat = format;
	mUsage = usage;

    unsigned int texelSize = sInternalTextureFormatSize[mFormat];
	unsigned int externalTexelSize = sExternalTextureFormatSize[mFormat];

    mLevelCount = 1;

	// allocate level for BeginLoadData/EndLoadData
	if (usage != kImmutableUsage)
	{
		mLevels = new Level[mLevelCount];

		mLevels[0].mData = NULL;
		mLevels[0].mWidth = mWidth;
		mLevels[0].mHeight = mHeight;
		mLevels[0].mSize = mWidth*mHeight*externalTexelSize;
	}

	D3D11_SUBRESOURCE_DATA* subresourcePtr = NULL;
	D3D11_SUBRESOURCE_DATA subresourceData;
	void* pixelData = data;
	if (data)
	{
		// if 24-bit color, we need to convert to 32-bit color
		if (data && kRGB24TexFmt == format)
		{
			pixelData = new unsigned char[4 * width * height];
			Convert24Bit(pixelData, data, width, height);
		}

		memset(&subresourceData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		subresourceData.pSysMem = pixelData;
		subresourceData.SysMemPitch = width;

		subresourcePtr = &subresourceData;
	}

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	switch (usage)
	{
	default:
	case kDefaultUsage:
		desc.Usage = D3D11_USAGE_DEFAULT;
		break;
	case kImmutableUsage:
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		break;
	case kDynamicUsage:
		desc.Usage = D3D11_USAGE_DYNAMIC;
		break;
	}
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (kDynamicUsage == usage)
	{
		//*** not sure if needed
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	if (FAILED(device->CreateTexture2D(&desc, subresourcePtr, &mTexturePtr)))
	{
		return false;
	}

	if (kRGB24TexFmt == format)
	{
		delete [] pixelData;
	}

	mD3DFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	// verify formats

	if (FAILED(device->CreateShaderResourceView(mTexturePtr, NULL, &mShaderResourceView)))
	{
		mTexturePtr->Release();
		mTexturePtr = 0;
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::CreateMipmapped()
//-------------------------------------------------------------------------------
// Texture initialization
//-------------------------------------------------------------------------------
bool
IvTextureDX11::CreateMipmapped(unsigned int width, unsigned int height, IvTextureFormat format,
                               void** data, unsigned int levels, IvDataUsage usage, ID3D11Device* device)
{
/*	if (width == 0 || height == 0 || mTexturePtr)
	{
		return false;
	}

	if (usage == kImmutableUsage && !data)
	{
		return false;
	}

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
#if 0
	if (FAILED(D3DXCreateTexture(device, mWidth, mHeight, mLevelCount, D3DUSAGE_AUTOGENMIPMAP, sD3DTextureFormat[format],
		D3DPOOL_MANAGED, &mTexturePtr)))
	{
		return false;
	}

	D3DSURFACE_DESC desc;
	mTexturePtr->GetLevelDesc(0, &desc);
	mD3DFormat = desc.Format;

	// verify formats
#endif
*/
	return true;
}


//-------------------------------------------------------------------------------
// @ IvTextureDX11::Destroy()
//-------------------------------------------------------------------------------
// Destroy!
//-------------------------------------------------------------------------------
void
IvTextureDX11::Destroy()
{
	if (mShaderResourceView)
	{
		mShaderResourceView->Release();
		mShaderResourceView = 0;
	}

	if ( mTexturePtr )
	{
		mTexturePtr->Release();
		mTexturePtr = 0;
	}
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::MakeActive()
//-------------------------------------------------------------------------------
// Binds the texture to the desired unit
//-------------------------------------------------------------------------------
void IvTextureDX11::MakeActive(unsigned int unit, ID3D11Device* device)
{
	// For now, just create sampler state every time
	// DX11 should check for duplicates and return matching handle
	D3D11_SAMPLER_DESC samplerDesc;
	memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = mFilter;
	samplerDesc.AddressU = mUAddrMode;
	samplerDesc.AddressV = mVAddrMode;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = mEnableMip ? mLevelCount : 0;

	ID3D11SamplerState* samplerState;
	if (S_OK != device->CreateSamplerState(&samplerDesc, &samplerState))
	{
		//*** some error state here?
		return;
	}
	ID3D11DeviceContext* d3dContext = ((IvRendererDX11*)IvRenderer::mRenderer)->GetContext();
	d3dContext->PSSetSamplers(unit, 1, &samplerState);

	d3dContext->PSSetShaderResources(unit, 1, &mShaderResourceView);
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::BeginLoadData()
//-------------------------------------------------------------------------------
// "Locks" and returns a pointer to sysmem texture data for the given level
//-------------------------------------------------------------------------------
void* IvTextureDX11::BeginLoadData(unsigned int level)
{
	if (kImmutableUsage == mUsage || level >= mLevelCount)
	{
		return NULL;
	}

	// already "locked"
	if (mLevels[level].mData)
	{
		return NULL;
	}

	mLevels[level].mData = (void*) new unsigned char[mLevels[level].mSize];
	return mLevels[level].mData;
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::EndLoadData()
//-------------------------------------------------------------------------------
// Uploads the edited texture data to D3D
//-------------------------------------------------------------------------------
bool  IvTextureDX11::EndLoadData(unsigned int level)
{
	if (kImmutableUsage == mUsage || level >= mLevelCount)
	{
		return false;
	}

	// not already "locked"
	if (!mLevels[level].mData)
	{
		return false;
	}

	// this is seriously ugly -- not clear how to easily get the context down here
	ID3D11DeviceContext* d3dContext = ((IvRendererDX11*)IvRenderer::mRenderer)->GetContext();
	if (kDefaultUsage == mUsage)
	{
		// use UpdateSubresource()

		void* pixelData = mLevels[level].mData;
		if (kRGB24TexFmt == mFormat)
		{
			pixelData = new unsigned char[4 * mLevels[level].mWidth * mLevels[level].mHeight];
			Convert24Bit(pixelData, mLevels[level].mData, mLevels[level].mWidth, mLevels[level].mHeight);
		}

		d3dContext->UpdateSubresource(mTexturePtr, level, NULL, pixelData, 4 * mLevels[level].mWidth, mLevels[level].mSize);

		if (kRGB24TexFmt == mFormat)
		{
			delete [] pixelData;
		}
	}
	else if (kDynamicUsage == mUsage)
	{
		// use Map/Unmap
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		//	Disable GPU access to the vertex buffer data.
		if (S_OK != d3dContext->Map(mTexturePtr, level, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
		{
			return false;
		}

		void* pixelData = mappedResource.pData;
		if (kRGB24TexFmt == mFormat)
		{
			Convert24Bit(pixelData, mLevels[level].mData, mLevels[level].mWidth, mLevels[level].mHeight);
		}
		else
		{
			memcpy(pixelData, mLevels[level].mData, mLevels[level].mSize);
		}

		//	Reenable GPU access to the vertex buffer data.
		d3dContext->Unmap(mTexturePtr, level);
	}

	delete [] mLevels[level].mData;
	mLevels[level].mData = NULL;

	return true;
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::SetAddressingU()
//-------------------------------------------------------------------------------
// Sets the texture wrapping in U
//-------------------------------------------------------------------------------
void IvTextureDX11::SetAddressingU(IvTextureAddrMode mode)
{
	mUAddrMode = (mode == kClampTexAddr) ? D3D11_TEXTURE_ADDRESS_CLAMP 
		                                 : D3D11_TEXTURE_ADDRESS_WRAP;
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::SetAddressingV()
//-------------------------------------------------------------------------------
// Sets the texture wrapping in V
//-------------------------------------------------------------------------------
void IvTextureDX11::SetAddressingV(IvTextureAddrMode mode)
{
	mVAddrMode = (mode == kClampTexAddr) ? D3D11_TEXTURE_ADDRESS_CLAMP 
		                                 : D3D11_TEXTURE_ADDRESS_WRAP;
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::SetMagFiltering()
//-------------------------------------------------------------------------------
// Sets the texture magnification filter
//-------------------------------------------------------------------------------
void IvTextureDX11::SetMagFiltering(IvTextureMagFilter filter)
{
	mMagFilter = filter;
	switch (mMinFilter)
	{
	case kNearestTexMinFilter:
		mFilter = (filter == kNearestTexMagFilter) 
			? D3D11_FILTER_MIN_MAG_MIP_POINT
			: D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		mEnableMip = false;
		break;
	case kBilerpTexMinFilter:
		mFilter = (filter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT
			: D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		mEnableMip = false;
		break;
	case kNearestMipmapNearestTexMinFilter:
		mFilter = (filter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_MAG_MIP_POINT
			: D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		mEnableMip = true;
		break;
	case kBilerpMipmapNearestTexMinFilter:
		mFilter = (filter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR
			: D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		mEnableMip = true;
		break;
	case kNearestMipmapLerpTexMinFilter:
		mFilter = (filter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT
			: D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		mEnableMip = true;
		break;
	case kBilerpMipmapLerpTexMinFilter:
		mFilter = (filter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
			: D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		mEnableMip = true;
		break;
	};	
}

//-------------------------------------------------------------------------------
// @ IvTextureDX11::SetMinFiltering()
//-------------------------------------------------------------------------------
// Sets the texture minification filter
//-------------------------------------------------------------------------------
void IvTextureDX11::SetMinFiltering(IvTextureMinFilter filter)
{
	mMinFilter = filter;
	switch (filter)
	{
	case kNearestTexMinFilter:
		mFilter = (mMagFilter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_MAG_MIP_POINT
			: D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		mEnableMip = false;
		break;
	case kBilerpTexMinFilter:
		mFilter = (mMagFilter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT
			: D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		mEnableMip = false;
		break;
	case kNearestMipmapNearestTexMinFilter:
		mFilter = (mMagFilter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_MAG_MIP_POINT
			: D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		mEnableMip = true;
		break;
	case kBilerpMipmapNearestTexMinFilter:
		mFilter = (mMagFilter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR
			: D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		mEnableMip = true;
		break;
	case kNearestMipmapLerpTexMinFilter:
		mFilter = (mMagFilter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT
			: D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		mEnableMip = true;
		break;
	case kBilerpMipmapLerpTexMinFilter:
		mFilter = (mMagFilter == kNearestTexMagFilter)
			? D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
			: D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		mEnableMip = true;
		break;
	};
}
