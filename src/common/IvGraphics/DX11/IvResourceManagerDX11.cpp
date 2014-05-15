//===============================================================================
// @ IvResourceManagerDX11.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvResourceManagerDX11.h"
#include "IvVertexBufferDX11.h"
#include "IvIndexBufferDX11.h"
#include "IvVertexShaderDX11.h"
#include "IvFragmentShaderDX11.h"
#include "IvShaderProgramDX11.h"
#include "IvTextureDX11.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::IvResourceManagerDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvResourceManagerDX11::IvResourceManagerDX11(ID3D11Device* device) : IvResourceManager()
{
	mDevice = device;
	mDevice->AddRef();
}	// End of IvResourceManagerDX11::IvResourceManagerDX11()

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::~IvResourceManagerDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvResourceManagerDX11::~IvResourceManagerDX11()
{
	mDevice->Release();
}	// End of IvResourceManagerDX11::~IvResourceManagerDX11()

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateVertexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex buffer
//-------------------------------------------------------------------------------
IvVertexBuffer* 
IvResourceManagerDX11::CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices, void* data )
{
    IvVertexBufferDX11* newBuffer = new IvVertexBufferDX11();
    if ( !newBuffer->Create( format, numVertices, data, mDevice ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}


//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerDX11::Destroy( IvVertexBuffer* vb)
{
    IvVertexBufferDX11* vbDX11 = static_cast<IvVertexBufferDX11*>(vb);
    vbDX11->Destroy();
    delete vbDX11;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateIndexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent index buffer
//-------------------------------------------------------------------------------
IvIndexBuffer* 
IvResourceManagerDX11::CreateIndexBuffer( unsigned int numIndices, void* data )
{
    IvIndexBufferDX11* newBuffer = new IvIndexBufferDX11();
    if ( !newBuffer->Create( numIndices, data, mDevice ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerDX11::Destroy( IvIndexBuffer* ib)
{
    IvIndexBufferDX11* ibDX11 = static_cast<IvIndexBufferDX11*>(ib);
    ibDX11->Destroy();
    delete ibDX11;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateVertexShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerDX11::CreateVertexShaderFromFile( const char* filename )
{
    IvVertexShaderDX11* vertexShader = new IvVertexShaderDX11();
    if ( !vertexShader->CreateFromFile( filename, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateVertexShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerDX11::CreateVertexShaderFromString( const char* string )
{
    IvVertexShaderDX11* vertexShader = new IvVertexShaderDX11();
    if ( !vertexShader->CreateFromString( string, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateVertexShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerDX11::CreateDefaultVertexShader( IvVertexFormat format )
{
    IvVertexShaderDX11* vertexShader = new IvVertexShaderDX11();
    if ( !vertexShader->CreateDefault( format, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerDX11::Destroy( IvVertexShader* vs)
{
    if ( vs )
    {
        IvVertexShaderDX11* vsDX11 = static_cast<IvVertexShaderDX11*>(vs);
        vsDX11->Destroy();
        delete vsDX11;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateFragmentShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerDX11::CreateFragmentShaderFromFile( const char* filename )
{
    IvFragmentShaderDX11* fragmentShader = new IvFragmentShaderDX11();
    if ( !fragmentShader->CreateFromFile( filename, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateFragmentShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerDX11::CreateFragmentShaderFromString( const char* string )
{
    IvFragmentShaderDX11* fragmentShader = new IvFragmentShaderDX11();
    if ( !fragmentShader->CreateFromString( string, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateFragmentShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerDX11::CreateDefaultFragmentShader( IvVertexFormat format )
{
    IvFragmentShaderDX11* fragmentShader = new IvFragmentShaderDX11();
    if ( !fragmentShader->CreateDefault( format, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerDX11::Destroy( IvFragmentShader* fs)
{
    if ( fs )
    {
        IvFragmentShaderDX11* fsDX11 = static_cast<IvFragmentShaderDX11*>(fs);
        fsDX11->Destroy();
        delete fsDX11;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateShaderProgram()
//-------------------------------------------------------------------------------
// Create platform-dependent shader program
//-------------------------------------------------------------------------------
IvShaderProgram* 
IvResourceManagerDX11::CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs )
{
	// make sure we're not handed garbage
	if ( vs == 0 || fs == 0 )
		return 0;

    IvShaderProgramDX11* newProgram = new IvShaderProgramDX11();
    if ( !newProgram->Create( static_cast<IvVertexShaderDX11*>(vs), 
                              static_cast<IvFragmentShaderDX11*>(fs) ) )
    {
        delete newProgram;
        newProgram = 0;
    }
    return newProgram;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerDX11::Destroy( IvShaderProgram* sp )
{
    IvShaderProgramDX11* spDX11 = static_cast<IvShaderProgramDX11*>(sp);
    spDX11->Destroy();
    delete spDX11;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::CreateTexture()
//-------------------------------------------------------------------------------
// Create platform-dependent texture
//-------------------------------------------------------------------------------
IvTexture*
IvResourceManagerDX11::CreateTexture( IvTextureFormat format, 
    unsigned int width, unsigned int height )
{
/***	IvTextureDX11* newTexture = new IvTextureDX11();
    if ( !newTexture->Create( width, height, format, mDevice ) )
    {
        delete newTexture;
        newTexture = 0;
    }
    return newTexture;
	***/
	return NULL;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerDX11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent texture
//-------------------------------------------------------------------------------
void 
IvResourceManagerDX11::Destroy( IvTexture* tex )
{
/***    IvTextureDX11* texDX11 = static_cast<IvTextureDX11*>(tex);
    texDX11->Destroy();
    delete texDX11; ***/
}
