//===============================================================================
// @ IvImage.cpp
// 
// Image data loading and handling class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//===============================================================================

#include "IvImage.h"

#include <memory.h>
#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------------------
// @ IvReadBinary (unsigned char)
//-------------------------------------------------------------------------------
// Reads a byte from a file
//-------------------------------------------------------------------------------
static bool IvReadBinary(FILE* file, unsigned char& res)
{
    return (fread(&res, 1, 1, file) == 1) ? true : false;
}  // End of IvReadBinary


//-------------------------------------------------------------------------------
// @ IvWriteBinary (unsigned char)
//-------------------------------------------------------------------------------
// Writes a byte to a file
//-------------------------------------------------------------------------------
static bool IvWriteBinary(FILE* file, unsigned char res)
{
    return (fwrite(&res, 1, 1, file) == 1) ? true : false;
}  // End of IvWriteBinary


//-------------------------------------------------------------------------------
// @ IvReadBinary (unsigned short)
//-------------------------------------------------------------------------------
// Reads a byte from a file, handling the endian issues
//-------------------------------------------------------------------------------
static bool IvReadBinary(FILE* file, unsigned short& res)
{
    unsigned char temp[2];
    if (fread(temp, 1, 2, file) != 2)
        return false;

    // swap for endian differences - this assumes that the file is little-endian
    // but works for both cases of loading machine
    res = ((unsigned short)temp[0]) | (((unsigned short)temp[1]) << 8);
    return true;
}  // End of IvReadBinary


//-------------------------------------------------------------------------------
// @ IvImage::IvImage
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvImage::IvImage(unsigned int width, unsigned int height, 
    unsigned int components, unsigned int bytesPerComponent)
{
    mWidth = width;
    mHeight = height;
    mBytesPerComponent = bytesPerComponent;
    mComponents = components;
    mBytesPerPixel = bytesPerComponent * components;

    mPixels = new unsigned char[mWidth * mHeight * mBytesPerPixel];
}   // End of IvImage::IvImage


//-------------------------------------------------------------------------------
// @ IvImage::IvImage
//-------------------------------------------------------------------------------
// Copy Constructor
//-------------------------------------------------------------------------------
IvImage::IvImage(const IvImage& other)
{
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mBytesPerComponent = other.mBytesPerComponent;
    mComponents = other.mComponents;
    mBytesPerPixel = other.mBytesPerPixel;

    if (mPixels)
        delete[] mPixels;

    const unsigned int imageBytes = mWidth * mHeight * mBytesPerPixel;
    mPixels = new unsigned char[imageBytes];
    memcpy(mPixels, other.mPixels, imageBytes);
}   // End of IvImage::IvImage


//-------------------------------------------------------------------------------
// @ IvImage::~IvImage
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvImage::~IvImage()
{
    delete[] mPixels;
}   // End of IvImage::~IvImage


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvImage& source)
{
    out << source.mWidth << ' ' << source.mHeight << ' ';
    out << source.mBytesPerComponent << ' ' << source.mComponents << ' ';
    out << source.mBytesPerPixel <<  eol;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvImage::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvImage&
IvImage::operator=(const IvImage& other)
{
    // if same object
    if ( this == &other )
        return *this;
    
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mBytesPerComponent = other.mBytesPerComponent;
    mComponents = other.mComponents;
    mBytesPerPixel = other.mBytesPerPixel;

    if (mPixels)
        delete[] mPixels;

    const unsigned int imageBytes = mWidth * mHeight * mBytesPerPixel;
    mPixels = new unsigned char[imageBytes];
    memcpy(mPixels, other.mPixels, imageBytes);

    return *this;

}   // End of IvImage::operator=()


//-------------------------------------------------------------------------------
// @ IvImage::CreateFromFile
//-------------------------------------------------------------------------------
// Creates and returns an image representing the given file, or NULL if the file
// does not exist or is an unsupported format
//-------------------------------------------------------------------------------
IvImage* IvImage::CreateFromFile(const char* pcFilename)
{
    FILE* file = fopen(pcFilename, "rb");

    if (!file)
        return 0;

    unsigned char IDLength;
    if (!IvReadBinary(file, IDLength))
    {
        fclose(file);
        return 0;
    }

    unsigned char colorMapType;
    if (!IvReadBinary(file, colorMapType))
    {
        fclose(file);
        return 0;
    }

    unsigned char imageType;
    if (!IvReadBinary(file, imageType))
    {
        fclose(file);
        return 0;
    }

    // skip 5 bytes of palette info
    fseek(file, 5, SEEK_CUR);

    // only care that this is zero
    unsigned short xOrigin;
    if (!IvReadBinary(file, xOrigin))
    {
        fclose(file);
        return 0;
    }
    
    // only care that this is zero
    unsigned short yOrigin;
    if (!IvReadBinary(file, yOrigin))
    {
        fclose(file);
        return 0;
    }

    // MUST byte swap these as needed for the platform
    unsigned short shortWidth;
    if (!IvReadBinary(file, shortWidth))
    {
        fclose(file);
        return 0;
    }

    unsigned short shortHeight;
    if (!IvReadBinary(file, shortHeight))
    {
        fclose(file);
        return 0;
    }

    unsigned char pixelSize;
    if (!IvReadBinary(file, pixelSize))
    {
        fclose(file);
        return 0;
    }

    unsigned char imageAttributes;
    if (!IvReadBinary(file, imageAttributes))
    {
        fclose(file);
        return 0;
    }

    // skip ID field
    if (IDLength != 0)
        fseek(file, IDLength, SEEK_CUR);

    if (imageType != 2)
    {
        fclose(file);
        return 0;
    }

    unsigned int bytesPerPixel = pixelSize >> 3;
    unsigned int width = shortWidth;
    unsigned int height = shortHeight;

    IvImage* image = 0;

    if (pixelSize == 32)
    {
        // RGBA
        image = new IvImage(width, height, 4, 1);        
    }
    else if (pixelSize == 24)
    {
        // RGB
        image = new IvImage(width, height, 3, 1);        
    }
    else
    {
        fclose(file);
        return 0;
    }
 
    unsigned char* pixels = image->GetPixels();

    unsigned int rowSize = bytesPerPixel * width;
    int rowStep = (int)rowSize;

    // Do not load right-to-left images
    if (imageAttributes & 0x10)
    {
        fclose(file);
        delete image;
        return 0;
    }

    // Must flip top-to-bottom images
    if (imageAttributes & 0x20)
    {
        pixels += rowSize * (height - 1);
        rowStep = -rowStep;
    }

    for (unsigned int row = 0; row < height; row++)
    {
        // Load a row of raw pixels into the image directly 
        // from the image file
        fread(pixels, rowSize, 1, file);

        unsigned char* tempRow = pixels;

        // Pixels are read in as BGR(A), and we transpose to RGB(A) 
        for (unsigned int i = 0; i < width; i++)
        {
            unsigned char temp = tempRow[2];
            tempRow[2] = tempRow[0];
            tempRow[0] = temp;

            tempRow += bytesPerPixel;
        }

        pixels += rowStep;
    }

    return image;
}   // End of IvImage::CreateFromFile
