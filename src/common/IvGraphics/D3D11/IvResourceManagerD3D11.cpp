//===============================================================================
// @ IvResourceManagerD3D11.cpp
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

#include "IvResourceManagerD3D11.h"
#include "IvVertexBufferD3D11.h"
#include "IvIndexBufferD3D11.h"
#include "IvVertexShaderD3D11.h"
#include "IvFragmentShaderD3D11.h"
#include "IvShaderProgramD3D11.h"
#include "IvTextureD3D11.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::IvResourceManagerD3D11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvResourceManagerD3D11::IvResourceManagerD3D11(ID3D11Device* device) : IvResourceManager()
{
    mDevice = device;
    mDevice->AddRef();
}   // End of IvResourceManagerD3D11::IvResourceManagerD3D11()

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::~IvResourceManagerD3D11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvResourceManagerD3D11::~IvResourceManagerD3D11()
{
    mDevice->Release();
}   // End of IvResourceManagerD3D11::~IvResourceManagerD3D11()

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateVertexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex buffer
//-------------------------------------------------------------------------------
IvVertexBuffer* 
IvResourceManagerD3D11::CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices, 
                                          void* data, IvDataUsage usage  )
{
    IvVertexBufferD3D11* newBuffer = new IvVertexBufferD3D11();
    if ( !newBuffer->Create( format, numVertices, data, usage, mDevice ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}


//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D11::Destroy( IvVertexBuffer* vb)
{
    IvVertexBufferD3D11* vbD3D11 = static_cast<IvVertexBufferD3D11*>(vb);
    vbD3D11->Destroy();
    delete vbD3D11;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateIndexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent index buffer
//-------------------------------------------------------------------------------
IvIndexBuffer* 
IvResourceManagerD3D11::CreateIndexBuffer(unsigned int numIndices, void* data, IvDataUsage usage)
{
    IvIndexBufferD3D11* newBuffer = new IvIndexBufferD3D11();
    if ( !newBuffer->Create( numIndices, data, usage, mDevice ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D11::Destroy( IvIndexBuffer* ib)
{
    IvIndexBufferD3D11* ibD3D11 = static_cast<IvIndexBufferD3D11*>(ib);
    ibD3D11->Destroy();
    delete ibD3D11;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateVertexShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerD3D11::CreateVertexShaderFromFile( const char* filename )
{
    IvVertexShaderD3D11* vertexShader = new IvVertexShaderD3D11();
    if ( !vertexShader->CreateFromFile( filename, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateVertexShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerD3D11::CreateVertexShaderFromString( const char* string )
{
    IvVertexShaderD3D11* vertexShader = new IvVertexShaderD3D11();
    if ( !vertexShader->CreateFromString( string, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateVertexShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerD3D11::CreateDefaultVertexShader( IvVertexFormat format )
{
    IvVertexShaderD3D11* vertexShader = new IvVertexShaderD3D11();
    if ( !vertexShader->CreateDefault( format, mDevice ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D11::Destroy( IvVertexShader* vs)
{
    if ( vs )
    {
        IvVertexShaderD3D11* vsD3D11 = static_cast<IvVertexShaderD3D11*>(vs);
        vsD3D11->Destroy();
        delete vsD3D11;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateFragmentShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerD3D11::CreateFragmentShaderFromFile( const char* filename )
{
    IvFragmentShaderD3D11* fragmentShader = new IvFragmentShaderD3D11();
    if ( !fragmentShader->CreateFromFile( filename, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateFragmentShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerD3D11::CreateFragmentShaderFromString( const char* string )
{
    IvFragmentShaderD3D11* fragmentShader = new IvFragmentShaderD3D11();
    if ( !fragmentShader->CreateFromString( string, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateFragmentShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerD3D11::CreateDefaultFragmentShader( IvVertexFormat format )
{
    IvFragmentShaderD3D11* fragmentShader = new IvFragmentShaderD3D11();
    if ( !fragmentShader->CreateDefault( format, mDevice ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D11::Destroy( IvFragmentShader* fs)
{
    if ( fs )
    {
        IvFragmentShaderD3D11* fsD3D11 = static_cast<IvFragmentShaderD3D11*>(fs);
        fsD3D11->Destroy();
        delete fsD3D11;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateShaderProgram()
//-------------------------------------------------------------------------------
// Create platform-dependent shader program
//-------------------------------------------------------------------------------
IvShaderProgram* 
IvResourceManagerD3D11::CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs )
{
    // make sure we're not handed garbage
    if ( vs == 0 || fs == 0 )
        return 0;

    IvShaderProgramD3D11* newProgram = new IvShaderProgramD3D11();
    if ( !newProgram->Create( static_cast<IvVertexShaderD3D11*>(vs), 
                              static_cast<IvFragmentShaderD3D11*>(fs) ) )
    {
        delete newProgram;
        newProgram = 0;
    }

    Destroy(vs);
    Destroy(fs);

    return newProgram;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D11::Destroy( IvShaderProgram* sp )
{
    IvShaderProgramD3D11* spD3D11 = static_cast<IvShaderProgramD3D11*>(sp);
    spD3D11->Destroy();
    delete spD3D11;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateTexture()
//-------------------------------------------------------------------------------
// Create platform-dependent texture
//-------------------------------------------------------------------------------
IvTexture*
IvResourceManagerD3D11::CreateTexture(IvTextureFormat format, 
                                     unsigned int width, unsigned int height, 
                                     void* data, IvDataUsage usage)
{
    IvTextureD3D11* newTexture = new IvTextureD3D11();
    if (!newTexture->Create(width, height, format, data, usage, mDevice))
    {
        delete newTexture;
        newTexture = 0;
    }
    return newTexture;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::CreateMipmappedTexture()
//-------------------------------------------------------------------------------
// Create platform-dependent texture
//-------------------------------------------------------------------------------
IvTexture*
IvResourceManagerD3D11::CreateMipmappedTexture(IvTextureFormat format,
       unsigned int width, unsigned int height,
       void** data, unsigned int levels, IvDataUsage usage)
{
    IvTextureD3D11* newTexture = new IvTextureD3D11();
    if (!newTexture->CreateMipmapped(width, height, format, data, levels, usage, mDevice))
    {
        delete newTexture;
        newTexture = 0;
    }
    return newTexture;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerD3D11::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent texture
//-------------------------------------------------------------------------------
void 
IvResourceManagerD3D11::Destroy( IvTexture* tex )
{
    IvTextureD3D11* texD3D11 = static_cast<IvTextureD3D11*>(tex);
    texD3D11->Destroy();
    delete texD3D11;
}
