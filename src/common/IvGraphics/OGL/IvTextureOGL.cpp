//===============================================================================
// @ IvTextureOGL.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#include "IvTextureOGL.h"
#ifdef PLATFORM_OSX
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

static unsigned int sTextureFormatSize[kTexFmtCount] = {4, 3};

//-------------------------------------------------------------------------------
// @ IvTextureOGL::IvTextureOGL()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvTextureOGL::IvTextureOGL() : IvTexture()
    , mID(0)
    , mLevels(0)
    , mLevelCount(0)
{
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::~IvTextureOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvTextureOGL::~IvTextureOGL()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::IvTextureOGL()
//-------------------------------------------------------------------------------
// Texture initialization
//-------------------------------------------------------------------------------
bool
IvTextureOGL::Create(unsigned int width, unsigned int height, IvTextureFormat format)
{
    glGenTextures(1, &mID);

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

    mLevels[0].mData = (void*)(new unsigned char[size]);
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

        mLevels[i].mData = (void*)(((unsigned char*)(mLevels[i-1].mData)) + size);
        mLevels[i].mWidth = width;
        mLevels[i].mHeight = height;
        mLevels[i].mSize = size;
    }

	return true;
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::Destroy()
//-------------------------------------------------------------------------------
// Clean up
//-------------------------------------------------------------------------------
void
IvTextureOGL::Destroy()
{
    if (mID != 0)
    {
        glDeleteTextures(1, &mID);
        delete[] (unsigned char*)(mLevels[0].mData);
        delete[] mLevels;
        mID = 0;
    }
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::MakeActive()
//-------------------------------------------------------------------------------
// Binds the texture to the desired unit
//-------------------------------------------------------------------------------
void IvTextureOGL::MakeActive(unsigned int unit)
{
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, mID);
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::BeginLoadData()
//-------------------------------------------------------------------------------
// "Locks" and returns a pointer to the sysmem texture data for the given level
//-------------------------------------------------------------------------------
void* IvTextureOGL::BeginLoadData(unsigned int level)
{
    return mLevels[level].mData;
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::EndLoadData()
//-------------------------------------------------------------------------------
// Uploads the edited texture data to GL
//-------------------------------------------------------------------------------
bool  IvTextureOGL::EndLoadData(unsigned int level)
{
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);

    switch (mFormat)
    {
        case kRGBA32TexFmt:
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 
                mLevels[level].mWidth, mLevels[level].mHeight, 0, 
                GL_RGBA, GL_UNSIGNED_BYTE, mLevels[level].mData);
            break;

        case kRGB24TexFmt:
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, 
                mLevels[level].mWidth, mLevels[level].mHeight, 0, 
                GL_RGB, GL_UNSIGNED_BYTE, mLevels[level].mData);
            break;
            
        default:
            break;
    };

    glBindTexture(GL_TEXTURE_2D, currentTex);
    return true;
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::SetAddressingU()
//-------------------------------------------------------------------------------
// Sets the texture wrapping in U
//-------------------------------------------------------------------------------
void IvTextureOGL::SetAddressingU(IvTextureAddrMode mode)
{
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        (mode == kClampTexAddr) ? GL_CLAMP_TO_EDGE : GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, currentTex);
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::SetAddressingV()
//-------------------------------------------------------------------------------
// Sets the texture wrapping in V
//-------------------------------------------------------------------------------
void IvTextureOGL::SetAddressingV(IvTextureAddrMode mode)
{
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
        (mode == kClampTexAddr) ? GL_CLAMP_TO_EDGE : GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, currentTex);
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::SetMagFiltering()
//-------------------------------------------------------------------------------
// Sets the texture magnification filter
//-------------------------------------------------------------------------------
void IvTextureOGL::SetMagFiltering(IvTextureMagFilter filter)
{
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        (filter == kNearestTexMagFilter) ? GL_NEAREST : GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, currentTex);
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::SetMinFiltering()
//-------------------------------------------------------------------------------
// Sets the texture minification filter
//-------------------------------------------------------------------------------
void IvTextureOGL::SetMinFiltering(IvTextureMinFilter filter)
{
    GLint mode;

    switch(filter)
    {
        case kNearestTexMinFilter:
            mode = GL_NEAREST;
            break;
        case kBilerpTexMinFilter:
            mode = GL_LINEAR;
            break;
        case kNearestMipmapNearestTexMinFilter:
            mode = GL_NEAREST_MIPMAP_NEAREST;
            break;
        case kBilerpMipmapNearestTexMinFilter:
            mode = GL_LINEAR_MIPMAP_NEAREST;
            break;
        case kNearestMipmapLerpTexMinFilter:
            mode = GL_NEAREST_MIPMAP_LINEAR;
            break;
        case kBilerpMipmapLerpTexMinFilter:
            mode = GL_LINEAR_MIPMAP_LINEAR;
            break;
    };

    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);

    glBindTexture(GL_TEXTURE_2D, currentTex);
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::GenerateMipmapPyramid()
//-------------------------------------------------------------------------------
// Fills the mipmap levels based on the finest level (0)
//-------------------------------------------------------------------------------
void IvTextureOGL::GenerateMipmapPyramid()
{
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);
/**** FIX THIS
    switch (mFormat)
    {
        case kRGBA32TexFmt:
            gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, 
                mLevels[0].mWidth, mLevels[0].mHeight, GL_RGBA, 
                GL_UNSIGNED_BYTE, mLevels[0].mData);
            break;

        case kRGB24TexFmt:
            gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, 
                mLevels[0].mWidth, mLevels[0].mHeight, GL_RGB, 
                GL_UNSIGNED_BYTE, mLevels[0].mData);
            break;
    };
*/
    glBindTexture(GL_TEXTURE_2D, currentTex);
}

