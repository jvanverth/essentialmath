//===============================================================================
// @ IvResourceManagerD3D9.h
// 
// D3D11 implementation of resource manager
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvResourceManagerD3D9__h__
#define __IvResourceManagerD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvResourceManager.h"
#include "../IvVertexFormats.h"
#include <d3d11.h>

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

class IvResourceManagerD3D11 : public IvResourceManager
{ 
public:
    IvVertexBuffer* CreateVertexBuffer(IvVertexFormat format, unsigned int numVertices, 
                                       void* data, IvDataUsage usage) final;
    void Destroy(IvVertexBuffer* vb) final;
    
    IvIndexBuffer* CreateIndexBuffer(unsigned int numIndices, void* data, IvDataUsage usage) final;
    void Destroy(IvIndexBuffer* ib) final;
    
    IvVertexShader* CreateVertexShaderFromFile(const char* filename) final;
    IvVertexShader* CreateVertexShaderFromString(const char* string) final;
    IvVertexShader* CreateDefaultVertexShader(IvVertexFormat format) final;
    void Destroy(IvVertexShader* vs) final;
    
    IvFragmentShader* CreateFragmentShaderFromFile(const char* filename) final;
    IvFragmentShader* CreateFragmentShaderFromString(const char* string) final;
    IvFragmentShader* CreateDefaultFragmentShader(IvVertexFormat format) final;
    void Destroy(IvFragmentShader* vs) final;
    
    IvShaderProgram* CreateShaderProgram(IvVertexShader* vs, IvFragmentShader* fs) final;
    void Destroy(IvShaderProgram* sp) final;

    virtual IvTexture* CreateTexture(IvTextureFormat format, 
        unsigned int width, unsigned int height, 
        void* data, IvDataUsage usage) final;
    virtual IvTexture* CreateMipmappedTexture(IvTextureFormat format,
        unsigned int width, unsigned int height,
        void** data, unsigned int levels, IvDataUsage usage);
    void Destroy(IvTexture* tex) final;
    
private: 
    IvResourceManagerD3D11(ID3D11Device* device);
    ~IvResourceManagerD3D11() final;
    
    friend class IvRendererD3D11;
    
private: 
    // private D3D9 things 
    ID3D11Device* mDevice;
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif