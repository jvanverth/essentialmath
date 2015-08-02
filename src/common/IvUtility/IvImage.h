//===============================================================================
// @ IvImage.h
// 
// Image data loading and handling class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//
//===============================================================================

#ifndef __IvImage__h__
#define __IvImage__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvWriter.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------
class IvImage
{
public:
    IvImage(unsigned int width, unsigned int height, unsigned int components,
        unsigned int bytesPerComponent);
    IvImage(const IvImage& other);

    ~IvImage();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvImage& source );

    IvImage& operator=(const IvImage& other);

    const unsigned char* GetPixels() const { return mPixels; }
    unsigned char* GetPixels() { return mPixels; }

    unsigned int GetWidth() const { return mWidth; }
    unsigned int GetHeight() const { return mHeight; }
    unsigned int GetBytesPerComponent() const { return mBytesPerComponent; }
    unsigned int GetComponents() const { return mComponents; }
    unsigned int GetBytesPerPixel() const { return mBytesPerPixel; }

    const unsigned char* GetPixel(unsigned int x, unsigned int y) const
    { return mPixels + mBytesPerPixel * (x + y * mWidth); }
    unsigned char* GetPixel(unsigned int x, unsigned int y)
    { return mPixels + mBytesPerPixel * (x + y * mWidth); }

    // Currently, supports only 24- and 32-bit uncompressed RGB(A) TARGA files
    static IvImage* CreateFromFile(const char* pcFilename);

private:
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mBytesPerComponent;
    unsigned int mComponents;
    unsigned int mBytesPerPixel; // derived from mBytesPerComponent times
                                 // mComponents

    unsigned char* mPixels;
};

#endif
