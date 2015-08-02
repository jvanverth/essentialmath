//===============================================================================
// @ IvResourceManager.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Usage notes
//===============================================================================

#ifndef __IvResourceManager__h__
#define __IvResourceManager__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVertexFormats.h"
#include "IvTextureFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBuffer;
class IvIndexBuffer;
class IvVertexShader;
class IvFragmentShader;
class IvShaderProgram;
class IvTexture;

enum IvDataUsage
{
    kImmutableUsage,
    kDefaultUsage,
    kDynamicUsage
};

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvResourceManager
{ 
public:
    virtual IvVertexBuffer* CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices,
                                               void* data, IvDataUsage usage ) = 0;
    virtual void Destroy( IvVertexBuffer* vb ) = 0;

    virtual IvIndexBuffer* CreateIndexBuffer( unsigned int numIndices, void* data,
                                              IvDataUsage usage ) = 0;
    virtual void Destroy( IvIndexBuffer* ib ) = 0;

    virtual IvVertexShader* CreateVertexShaderFromFile( const char* filename ) = 0;
    virtual IvVertexShader* CreateVertexShaderFromString( const char* string ) = 0;
    virtual IvVertexShader* CreateDefaultVertexShader( IvVertexFormat format ) = 0;
    virtual void Destroy( IvVertexShader* vs ) = 0;

    virtual IvFragmentShader* CreateFragmentShaderFromFile( const char* filename ) = 0;
    virtual IvFragmentShader* CreateFragmentShaderFromString( const char* string ) = 0;
    virtual IvFragmentShader* CreateDefaultFragmentShader( IvVertexFormat format ) = 0;
    virtual void Destroy( IvFragmentShader* vs ) = 0;

    virtual IvShaderProgram* CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs ) = 0;
    virtual void Destroy( IvShaderProgram* sp ) = 0;

    virtual IvTexture* CreateTexture(IvTextureFormat format,
                                     unsigned int width, unsigned int height,
                                     void* data, IvDataUsage usage) = 0;
    virtual IvTexture* CreateMipmappedTexture(IvTextureFormat format,
                                     unsigned int width, unsigned int height,
                                     void** data, unsigned int levels, IvDataUsage usage) = 0;
    virtual void Destroy(IvTexture* tex) = 0;

protected:
    IvResourceManager() {}
    virtual ~IvResourceManager() {}
    
private:
    // copy operations
    IvResourceManager(const IvVertexBuffer& other);
    IvResourceManager& operator=(const IvVertexBuffer& other);
    
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
