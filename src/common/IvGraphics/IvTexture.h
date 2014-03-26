//===============================================================================
// @ IvTexture.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvTexture__h__
#define __IvTexture__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvTextureFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

enum IvTextureAddrMode
{
    kClampTexAddr,
    kWrapTexAddr
};

enum IvTextureMagFilter
{
    kNearestTexMagFilter,
    kBilerpTexMagFilter
};

enum IvTextureMinFilter
{
    kNearestTexMinFilter,
    kBilerpTexMinFilter,
    kNearestMipmapNearestTexMinFilter,
    kBilerpMipmapNearestTexMinFilter,
    kNearestMipmapLerpTexMinFilter,
    kBilerpMipmapLerpTexMinFilter
};

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvTexture
{
public:
    // interface routines
    virtual void* BeginLoadData(unsigned int level = 0) = 0;
    virtual bool  EndLoadData(unsigned int level = 0) = 0;

    virtual void SetAddressingU(IvTextureAddrMode mode) = 0;
    virtual void SetAddressingV(IvTextureAddrMode mode) = 0;

    virtual void SetMagFiltering(IvTextureMagFilter filter) = 0;
    virtual void SetMinFiltering(IvTextureMinFilter filter) = 0;

    virtual void GenerateMipmapPyramid() = 0;

    inline IvTextureFormat GetFormat() const { return mFormat; }
    inline unsigned int GetWidth() const { return mWidth; }
    inline unsigned int GetHeight() const { return mHeight; }

protected:
    // constructor/destructor
	IvTexture() : mFormat(kRGBA32TexFmt), mWidth(0), mHeight(0) { }
    virtual ~IvTexture() {}

    IvTextureFormat     mFormat;
    unsigned int        mWidth;
    unsigned int        mHeight;

private:
    // copy operations (unimplemented so we can't copy)
    IvTexture(const IvTexture& other);
	IvTexture& operator=(const IvTexture& other);
}; 

#endif
