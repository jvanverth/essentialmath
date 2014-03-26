//===============================================================================
// @ IvResourceManagerD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvResourceManagerD3D9__h__
#define __IvResourceManagerD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvResourceManager.h"
#include "../IvVertexFormats.h"
#include <d3d9.h>

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

class IvResourceManagerD3D9 : public IvResourceManager
{ 
public:
    IvVertexBuffer* CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices ); 
    void Destroy( IvVertexBuffer* vb); 
    
    IvIndexBuffer* CreateIndexBuffer( unsigned int numIndices );
    void Destroy( IvIndexBuffer* ib); 
    
    IvVertexShader* CreateVertexShaderFromFile( const char* filename );
    IvVertexShader* CreateVertexShaderFromString( const char* string );
    IvVertexShader* CreateDefaultVertexShader( IvVertexFormat format );
    void Destroy( IvVertexShader* vs); 
    
    IvFragmentShader* CreateFragmentShaderFromFile( const char* filename );
    IvFragmentShader* CreateFragmentShaderFromString( const char* string );
    IvFragmentShader* CreateDefaultFragmentShader( IvVertexFormat format );
    void Destroy( IvFragmentShader* vs); 
    
    IvShaderProgram* CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs );
    void Destroy( IvShaderProgram* sp );

    virtual IvTexture* CreateTexture( IvTextureFormat format, 
        unsigned int width, unsigned int height );
    void Destroy( IvTexture* tex );
    
private: 
    IvResourceManagerD3D9( IDirect3DDevice9* device );
    ~IvResourceManagerD3D9();
    
    friend class IvRendererD3D9;
    
private: 
    // private D3D9 things 
	IDirect3DDevice9* mDevice;
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif