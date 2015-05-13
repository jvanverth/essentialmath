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

class IvResourceManagerDX11 : public IvResourceManager
{ 
public:
    IvVertexBuffer* CreateVertexBuffer(IvVertexFormat format, unsigned int numVertices, 
		                               void* data, IvDataUsage usage);
    void Destroy( IvVertexBuffer* vb); 
    
	IvIndexBuffer* CreateIndexBuffer(unsigned int numIndices, void* data, IvDataUsage usage);
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
		unsigned int width, unsigned int height, 
		void* data, IvDataUsage usage);
	virtual IvTexture* CreateMipmappedTexture(IvTextureFormat format,
		unsigned int width, unsigned int height,
		void** data, unsigned int levels, IvDataUsage usage);
	void Destroy(IvTexture* tex);
    
private: 
	IvResourceManagerDX11(ID3D11Device* device);
    ~IvResourceManagerDX11();
    
    friend class IvRendererDX11;
    
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