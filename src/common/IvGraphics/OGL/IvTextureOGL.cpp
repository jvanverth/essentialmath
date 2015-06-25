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
IvTextureOGL::Create(unsigned int width, unsigned int height, IvTextureFormat format,
                     void* data, IvDataUsage usage)
{
    if ( width == 0 || height == 0 || mID != 0 )
    {
        return false;
    }
    
    if ( usage == kImmutableUsage && !data )
    {
        return false;
    }
    
    glGenTextures(1, &mID);

    mWidth = width;
    mHeight = height;
    mFormat = format;
    
    mLevelCount = 1;

    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex);
    
    glBindTexture(GL_TEXTURE_2D, mID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mLevelCount-1);
    
    if (data)
    {
        width = mWidth;
        height = mHeight;
        
        switch (mFormat)
        {
            case kRGBA32TexFmt:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                             width, height, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
                
            case kRGB24TexFmt:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                             width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, data);
                break;
                
            default:
                break;
        };
    }
    
    glBindTexture(GL_TEXTURE_2D, currentTex);
    
    mUsage = usage;
    mTempData = NULL;
    
	return true;
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::IvTextureOGL()
//-------------------------------------------------------------------------------
// Texture initialization
//-------------------------------------------------------------------------------
bool
IvTextureOGL::CreateMipmapped(unsigned int width, unsigned int height, IvTextureFormat format,
                              void** data, unsigned int levels, IvDataUsage usage)
{
    if ( width == 0 || height == 0 || mID != 0 )
    {
        return false;
    }
    
    if ( usage == kImmutableUsage && !data )
    {
        return false;
    }
    
    glGenTextures(1, &mID);
    
    mWidth = width;
    mHeight = height;
    mFormat = format;
    
    //*** TODO: should we specify levels?
    if (levels == 0)
    {
        mLevelCount = 1;
        while ((width != 1) || (height != 1))
        {
            width >>= 1;
            height >>= 1;
            
            if (!width)
                width = 1;
            
            if (!height)
                height = 1;
            
            mLevelCount++;
        }
    }
    else
    {
        mLevelCount = levels;
    }
    
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex);
    
    glBindTexture(GL_TEXTURE_2D, mID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mLevelCount-1);
    
    if (data)
    {
        width = mWidth;
        height = mHeight;
        
        void** dataPtr = data;
        
        unsigned int level;
        for (level = 0; level < mLevelCount; level++)
        {
            switch (mFormat)
            {
                case kRGBA32TexFmt:
                    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA,
                                 width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, *dataPtr);
                    break;
                    
                case kRGB24TexFmt:
                    glTexImage2D(GL_TEXTURE_2D, level, GL_RGB,
                                 width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, *dataPtr);
                    break;
                    
                default:
                    break;
            };
            
            width >>= 1;
            height >>= 1;
            
            if (!width)
                width = 1;
            
            if (!height)
                height = 1;
            
            ++dataPtr;
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, currentTex);
    
    mUsage = usage;
    mTempData = NULL;
    
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
    if (mUsage == kImmutableUsage || mTempData)
    {
        return NULL;
    }
    
    unsigned width = mWidth;
    unsigned height = mHeight;
    
    width >>= level;
    height >>= level;
    
    if (!width)
        width = 1;
    
    if (!height)
        height = 1;
    
    unsigned int texelSize = sTextureFormatSize[mFormat];
    unsigned int size = texelSize * width * height;
    
    mTempData = new unsigned char[size];
    
    return mTempData;
}

//-------------------------------------------------------------------------------
// @ IvTextureOGL::EndLoadData()
//-------------------------------------------------------------------------------
// Uploads the edited texture data to GL
//-------------------------------------------------------------------------------
bool  IvTextureOGL::EndLoadData(unsigned int level)
{
    if (mUsage == kImmutableUsage || !mTempData)
    {
        return false;
    }
    
    GLint currentTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTex); 

    glBindTexture(GL_TEXTURE_2D, mID);

    unsigned width = mWidth;
    unsigned height = mHeight;
    
    width >>= level;
    height >>= level;
    
    if (!width)
        width = 1;
    
    if (!height)
        height = 1;
    
    switch (mFormat)
    {
        case kRGBA32TexFmt:
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 
                width, height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, mTempData);
            break;

        case kRGB24TexFmt:
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, 
                width, height, 0,
                GL_RGB, GL_UNSIGNED_BYTE, mTempData);
            break;
            
        default:
            break;
    };

    glBindTexture(GL_TEXTURE_2D, currentTex);
    
    delete [] mTempData;
    mTempData = NULL;
    
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

