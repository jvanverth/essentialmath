//===============================================================================
// @ IvResourceManagerOGL.h
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

#ifndef __IvResourceManagerOGL__h__
#define __IvResourceManagerOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvResourceManager.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Forward Declarations -------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBuffer;
class IvIndexBuffer;
class IvVertexShader;
class IvFragmentShader;
class IvTexture;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvResourceManagerOGL : public IvResourceManager
{ 
public:
    IvVertexBuffer* CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices,
                                       void* data, IvDataUsage usage );
    void Destroy( IvVertexBuffer* vb );
    
    IvIndexBuffer* CreateIndexBuffer( unsigned int numIndices, void* data,
                                      IvDataUsage usage );
    void Destroy( IvIndexBuffer* ib );
    
    IvVertexShader* CreateVertexShaderFromFile( const char* filename );
    IvVertexShader* CreateVertexShaderFromString( const char* string );
    IvVertexShader* CreateDefaultVertexShader( IvVertexFormat format );
    void Destroy( IvVertexShader* vs );
    
    IvFragmentShader* CreateFragmentShaderFromFile( const char* filename );
    IvFragmentShader* CreateFragmentShaderFromString( const char* string );
    IvFragmentShader* CreateDefaultFragmentShader( IvVertexFormat format );
    void Destroy( IvFragmentShader* vs );
    
    IvShaderProgram* CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs );
    void Destroy( IvShaderProgram* sp );

    virtual IvTexture* CreateTexture(IvTextureFormat format,
                                     unsigned int width, unsigned int height,
                                     void* data, IvDataUsage usage);
    virtual IvTexture* CreateMipmappedTexture(IvTextureFormat format,
                                              unsigned int width, unsigned int height,
                                              void** data, unsigned int levels, IvDataUsage usage);
    void Destroy( IvTexture* tex );
    
private: 
    IvResourceManagerOGL();
    ~IvResourceManagerOGL();
    
    friend class IvRendererOGL;
    
private: 
    // private OGL things 
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
