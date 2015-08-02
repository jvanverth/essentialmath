//===============================================================================
// @ IvResourceManagerOGL.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvResourceManagerOGL.h"
#include "IvVertexBufferOGL.h"
#include "IvIndexBufferOGL.h"
#include "IvVertexShaderOGL.h"
#include "IvFragmentShaderOGL.h"
#include "IvShaderProgramOGL.h"
#include "IvTextureOGL.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::IvResourceManagerOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvResourceManagerOGL::IvResourceManagerOGL() : IvResourceManager()
{
}   // End of IvResourceManagerOGL::IvResourceManagerOGL()

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::~IvResourceManagerOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvResourceManagerOGL::~IvResourceManagerOGL()
{
}   // End of IvResourceManagerOGL::~IvResourceManagerOGL()

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateVertexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex buffer
//-------------------------------------------------------------------------------
IvVertexBuffer* 
IvResourceManagerOGL::CreateVertexBuffer( IvVertexFormat format, unsigned int numVertices,
                                          void* data, IvDataUsage usage )
{
    IvVertexBufferOGL* newBuffer = new IvVertexBufferOGL();
    if ( !newBuffer->Create( format, numVertices, data, usage ) )
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerOGL::Destroy( IvVertexBuffer* vb)
{
    IvVertexBufferOGL* vbOGL = static_cast<IvVertexBufferOGL*>(vb);
    vbOGL->Destroy();
    delete vbOGL;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateIndexBuffer()
//-------------------------------------------------------------------------------
// Create platform-dependent index buffer
//-------------------------------------------------------------------------------
IvIndexBuffer* 
IvResourceManagerOGL::CreateIndexBuffer( unsigned int numIndices, void* data,
                                         IvDataUsage usage)
{
    IvIndexBufferOGL* newBuffer = new IvIndexBufferOGL();
    if (!newBuffer->Create( numIndices, data, usage ))
    {
        delete newBuffer;
        newBuffer = 0;
    }
    return newBuffer;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerOGL::Destroy( IvIndexBuffer* ib)
{
    IvIndexBufferOGL* ibOGL = static_cast<IvIndexBufferOGL*>(ib);
    ibOGL->Destroy();
    delete ibOGL;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateVertexShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerOGL::CreateVertexShaderFromFile( const char* filename )
{
    IvVertexShaderOGL* vertexShader = new IvVertexShaderOGL();
    if ( !vertexShader->CreateFromFile(  filename ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateVertexShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerOGL::CreateVertexShaderFromString( const char* string )
{
    IvVertexShaderOGL* vertexShader = new IvVertexShaderOGL();
    if ( !vertexShader->CreateFromString(  string ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateVertexShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvVertexShader* 
IvResourceManagerOGL::CreateDefaultVertexShader( IvVertexFormat format )
{
    IvVertexShaderOGL* vertexShader = new IvVertexShaderOGL();
    if ( !vertexShader->CreateDefault( format ) )
    {
        delete vertexShader;
        vertexShader = 0;
    }
    return vertexShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerOGL::Destroy( IvVertexShader* vs)
{
    if ( vs )
    {
        IvVertexShaderOGL* vsOGL = static_cast<IvVertexShaderOGL*>(vs);
        vsOGL->Destroy();
        delete vsOGL;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateFragmentShaderFromFile()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerOGL::CreateFragmentShaderFromFile( const char* filename )
{
    IvFragmentShaderOGL* fragmentShader = new IvFragmentShaderOGL();
    if ( !fragmentShader->CreateFromFile(  filename ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateFragmentShaderFromString()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerOGL::CreateFragmentShaderFromString( const char* string )
{
    IvFragmentShaderOGL* fragmentShader = new IvFragmentShaderOGL();
    if ( !fragmentShader->CreateFromString(  string ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateFragmentShader()
//-------------------------------------------------------------------------------
// Create platform-dependent vertex shader
//-------------------------------------------------------------------------------
IvFragmentShader* 
IvResourceManagerOGL::CreateDefaultFragmentShader( IvVertexFormat format )
{
    IvFragmentShaderOGL* fragmentShader = new IvFragmentShaderOGL();
    if ( !fragmentShader->CreateDefault( format ) )
    {
        delete fragmentShader;
        fragmentShader = 0;
    }
    return fragmentShader;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent vertex shader
//-------------------------------------------------------------------------------
void 
IvResourceManagerOGL::Destroy( IvFragmentShader* fs)
{
    if ( fs )
    {
        IvFragmentShaderOGL* fsOGL = static_cast<IvFragmentShaderOGL*>(fs);
        fsOGL->Destroy();
        delete fsOGL;
    }
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateShaderProgram()
//-------------------------------------------------------------------------------
// Create platform-dependent shader program
//-------------------------------------------------------------------------------
IvShaderProgram* 
IvResourceManagerOGL::CreateShaderProgram( IvVertexShader* vs, IvFragmentShader* fs )
{
    // make sure we're not handed garbage
    if ( vs == 0 || fs == 0 )
        return 0;

    IvShaderProgramOGL* newProgram = new IvShaderProgramOGL();
    if ( !newProgram->Create( static_cast<IvVertexShaderOGL*>(vs)->mShaderID, 
                              static_cast<IvFragmentShaderOGL*>(fs)->mShaderID ) )
    {
        delete newProgram;
        newProgram = 0;
    }
    return newProgram;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent index buffer
//-------------------------------------------------------------------------------
void 
IvResourceManagerOGL::Destroy( IvShaderProgram* sp )
{
    IvShaderProgramOGL* spOGL = static_cast<IvShaderProgramOGL*>(sp);
    spOGL->Destroy();
    delete spOGL;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateTexture()
//-------------------------------------------------------------------------------
// Create platform-dependent texture
//-------------------------------------------------------------------------------
IvTexture*
IvResourceManagerOGL::CreateTexture( IvTextureFormat format, 
                                     unsigned int width, unsigned int height,
                                     void* data, IvDataUsage usage )
{
    IvTextureOGL* newTexture = new IvTextureOGL();
    if ( !newTexture->Create( width, height, format, data, usage ) )
    {
        delete newTexture;
        newTexture = 0;
    }
    return newTexture;
}

//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::CreateMipmappedTexture()
//-------------------------------------------------------------------------------
// Create platform-dependent texture
//-------------------------------------------------------------------------------
IvTexture*
IvResourceManagerOGL::CreateMipmappedTexture(IvTextureFormat format,
                                             unsigned int width, unsigned int height,
                                             void** data, unsigned int levels, IvDataUsage usage )
{
    IvTextureOGL* newTexture = new IvTextureOGL();
    if ( !newTexture->CreateMipmapped( width, height, format, data, levels, usage ) )
    {
        delete newTexture;
        newTexture = 0;
    }
    return newTexture;
}
//-------------------------------------------------------------------------------
// @ IvResourceManagerOGL::Destroy()
//-------------------------------------------------------------------------------
// Delete platform-dependent texture
//-------------------------------------------------------------------------------
void 
IvResourceManagerOGL::Destroy( IvTexture* tex )
{
    IvTextureOGL* texOGL = static_cast<IvTextureOGL*>(tex);
    texOGL->Destroy();
    delete texOGL;
}

