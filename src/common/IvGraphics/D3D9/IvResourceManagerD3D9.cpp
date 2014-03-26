//===============================================================================
// @ IvResourceManagerD3D9.cpp
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

#include "IvResourceManagerD3D9.h"
#include "IvVertexBufferD3D9.h"
#include "IvIndexBufferD3D9.h"
#include "IvVertexShaderD3D9.h"
#include "IvFragmentShaderD3D9.h"
#include "IvShaderProgramD3D9.h"
#include "IvTextureD3D9.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::IvResourceManagerD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvResourceManagerD3D9::IvResourceManagerD3D9( IDirect3DDevice9* device ) : IvResourceManager()
{
	mDevice = device;
	mDevice->AddRef();
}	// End of IvResourceManagerD3D9::IvResourceManagerD3D9()

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::~IvResourceManagerD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvResourceManagerD3D9::~IvResourceManagerD3D9()
{
	mDevice->Release();
}	// End of IvResourceManagerD3D9::~IvResourceManagerD3D9()

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateVertexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex buffer
//-------------------------------------------------------------------------------
IvVertexBuffer* 
IvResourceManagerD3D9::CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices )
{
    IvVertexBufferD3D9* newBuffer = new IvVertexBufferD3D9();
    if ( !newBuffer->Create( format, numVertices, mDevice ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}


//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D9::Destroy( IvVertexBuffer* vb)
{
    IvVertexBufferD3D9* vbD3D9 = static_cast<IvVertexBufferD3D9*>(vb);
    vbD3D9->Destroy();
    delete vbD3D9;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateIndexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent index buffer
//-------------------------------------------------------------------------------
IvIndexBuffer* 
IvResourceManagerD3D9::CreateIndexBuffer( unsigned int numIndices )
{
    IvIndexBufferD3D9* newBuffer = new IvIndexBufferD3D9();
    if ( !newBuffer->Create( numIndices, mDevice ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D9::Destroy( IvIndexBuffer* ib)
{
    IvIndexBufferD3D9* ibD3D9 = static_cast<IvIndexBufferD3D9*>(ib);
    ibD3D9->Destroy();
    delete ibD3D9;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateVertexShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerD3D9::CreateVertexShaderFromFile( const char* filename )
{
    IvVertexShaderD3D9* vertexShader = new IvVertexShaderD3D9();
    if ( !vertexShader->CreateFromFile( filename, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateVertexShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerD3D9::CreateVertexShaderFromString( const char* string )
{
    IvVertexShaderD3D9* vertexShader = new IvVertexShaderD3D9();
    if ( !vertexShader->CreateFromString( string, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateVertexShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerD3D9::CreateDefaultVertexShader( IvVertexFormat format )
{
    IvVertexShaderD3D9* vertexShader = new IvVertexShaderD3D9();
    if ( !vertexShader->CreateDefault( format, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D9::Destroy( IvVertexShader* vs)
{
    if ( vs )
    {
        IvVertexShaderD3D9* vsD3D9 = static_cast<IvVertexShaderD3D9*>(vs);
        vsD3D9->Destroy();
        delete vsD3D9;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateFragmentShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerD3D9::CreateFragmentShaderFromFile( const char* filename )
{
    IvFragmentShaderD3D9* fragmentShader = new IvFragmentShaderD3D9();
    if ( !fragmentShader->CreateFromFile( filename, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateFragmentShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerD3D9::CreateFragmentShaderFromString( const char* string )
{
    IvFragmentShaderD3D9* fragmentShader = new IvFragmentShaderD3D9();
    if ( !fragmentShader->CreateFromString( string, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateFragmentShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerD3D9::CreateDefaultFragmentShader( IvVertexFormat format )
{
    IvFragmentShaderD3D9* fragmentShader = new IvFragmentShaderD3D9();
    if ( !fragmentShader->CreateDefault( format, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D9::Destroy( IvFragmentShader* fs)
{
    if ( fs )
    {
        IvFragmentShaderD3D9* fsD3D9 = static_cast<IvFragmentShaderD3D9*>(fs);
        fsD3D9->Destroy();
        delete fsD3D9;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateShaderProgram()
//-------------------------------------------------------------------------------
// Create platform-dependent shader program
//-------------------------------------------------------------------------------
IvShaderProgram* 
IvResourceManagerD3D9::CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs )
{
	// make sure we're not handed garbage
	if ( vs == 0 || fs == 0 )
		return 0;

    IvShaderProgramD3D9* newProgram = new IvShaderProgramD3D9();
    if ( !newProgram->Create( static_cast<IvVertexShaderD3D9*>(vs), 
                              static_cast<IvFragmentShaderD3D9*>(fs) ) )
    {
        delete newProgram;
        newProgram = 0;
    }
    return newProgram;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D9::Destroy( IvShaderProgram* sp )
{
    IvShaderProgramD3D9* spD3D9 = static_cast<IvShaderProgramD3D9*>(sp);
    spD3D9->Destroy();
    delete spD3D9;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::CreateTexture()
//-------------------------------------------------------------------------------
// Create platform-dependent texture
//-------------------------------------------------------------------------------
IvTexture*
IvResourceManagerD3D9::CreateTexture( IvTextureFormat format, 
    unsigned int width, unsigned int height )
{
	IvTextureD3D9* newTexture = new IvTextureD3D9();
    if ( !newTexture->Create( width, height, format, mDevice ) )
    {
        delete newTexture;
        newTexture = 0;
    }
    return newTexture;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D9::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent texture
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D9::Destroy( IvTexture* tex )
{
    IvTextureD3D9* texD3D9 = static_cast<IvTextureD3D9*>(tex);
    texD3D9->Destroy();
    delete texD3D9;
}
