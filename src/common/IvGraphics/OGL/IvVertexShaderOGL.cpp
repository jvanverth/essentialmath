//===============================================================================
// @ IvVertexShaderOGL.cpp
// 
// OpenGL vertex shader implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVertexShaderOGL.h"
#include "IvAssert.h"
#include "IvDebugger.h"
#include <string.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static const char sShaderHeader[] =
"#version 150\n"
"#extension GL_ARB_explicit_attrib_location : require\n"
"#define COLOR 0\n"
"#define NORMAL 1\n"
"#define TEXCOORD0 2\n"
"#define POSITION 3\n";

static char const* sDefaultVertexShader[kVertexFormatCount] = {0};

static const char sShaderPFormat[] =
"uniform mat4 IvModelViewProjectionMatrix;\n"
"layout(location = POSITION) in vec3 IvPos;"
"void main()\n"
"{\n"
"    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);\n"
"}\n";

static const char sShaderCPFormat[] =
"uniform mat4 IvModelViewProjectionMatrix;\n"
"layout(location = COLOR) in vec4 IvColor;"
"layout(location = POSITION) in vec3 IvPos;"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);\n"
"    color = IvColor;\n"
"}\n";

static const char sShaderNPFormat[] = 
"uniform mat4 IvModelViewProjectionMatrix;\n"
"uniform mat4 IvNormalMatrix;\n"
"uniform vec4 IvLightDirection;\n"
"uniform vec4 IvLightAmbient;\n"
"uniform vec4 IvLightDiffuse;\n"
"uniform vec4 IvDiffuseColor;\n"
"layout(location = NORMAL) in vec3 IvNormal;"
"layout(location = POSITION) in vec3 IvPos;"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);\n"
"    vec4 transNormal = normalize(IvNormalMatrix * vec4(IvNormal,0.0));\n"
"    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);\n"
"    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    color = IvDiffuseColor*lightValue;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"uniform mat4 IvModelViewProjectionMatrix;\n"
"uniform mat4 IvNormalMatrix;\n"
"uniform vec4 IvLightDirection;\n"
"uniform vec4 IvLightAmbient;\n"
"uniform vec4 IvLightDiffuse;\n"
"layout(location = COLOR) in vec4 IvColor;"
"layout(location = NORMAL) in vec3 IvNormal;"
"layout(location = POSITION) in vec3 IvPos;"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);\n"
"    vec4 transNormal = normalize(IvNormalMatrix * vec4(IvNormal,0.0));\n"
"    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);\n"
"    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    color = IvColor*lightValue;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"uniform mat4 IvModelViewProjectionMatrix;\n"
"layout(location = TEXCOORD0) in vec2 IvTexCoord0;"
"layout(location = COLOR) in vec4 IvColor;"
"layout(location = POSITION) in vec3 IvPos;"
"out vec2 uv;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);\n"
"    color = IvColor;\n"
"    uv = IvTexCoord0.xy;\n"
"}\n";

static const char sShaderTNPFormat[] = 
"uniform mat4 IvModelViewProjectionMatrix;\n"
"uniform mat4 IvNormalMatrix;\n"
"uniform vec4 IvLightDirection;\n"
"uniform vec4 IvLightAmbient;\n"
"uniform vec4 IvLightDiffuse;\n"
"layout(location = TEXCOORD0) in vec2 IvTexCoord0;"
"layout(location = NORMAL) in vec3 IvNormal;"
"layout(location = POSITION) in vec3 IvPos;"
"out vec2 uv;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = IvModelViewProjectionMatrix*vec4(IvPos,1.0);\n"
"    vec4 transNormal = normalize(IvNormalMatrix * vec4(IvNormal,0.0));\n"
"    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);\n"
"    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    color = lightValue;\n"
"    uv = IvTexCoord0.xy;\n"
"}\n";

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::IvVertexShaderOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvVertexShaderOGL::IvVertexShaderOGL() : IvVertexShader(), mShaderID(0)
{
    sDefaultVertexShader[kPFormat] = sShaderPFormat;
    sDefaultVertexShader[kCPFormat] = sShaderCPFormat;
    sDefaultVertexShader[kNPFormat] = sShaderNPFormat;
    sDefaultVertexShader[kCNPFormat] = sShaderCNPFormat;
    sDefaultVertexShader[kTCPFormat] = sShaderTCPFormat;
    sDefaultVertexShader[kTNPFormat] = sShaderTNPFormat;
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::~IvVertexShaderOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvVertexShaderOGL::~IvVertexShaderOGL()
{
    ASSERT(!mShaderID);
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::CreateFromFile()
//-------------------------------------------------------------------------------
// Create a shader from a file
//-------------------------------------------------------------------------------
bool
IvVertexShaderOGL::CreateFromFile( const char* filename )
{
    // Add the expected extension:
    std::string fullFilename = filename;

    fullFilename = fullFilename + std::string(".glslv");

    FILE* fp = fopen(fullFilename.c_str(), "r");

    if (!fp)
    {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    unsigned int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* shaderSrc = new char[length+1];

    length = fread(shaderSrc, 1, length, fp);
    shaderSrc[length] = 0;
    fclose(fp);

    // allocate the shader id
    mShaderID = glCreateShader( GL_VERTEX_SHADER );
    if (mShaderID == 0)
    {
        return false;
    }
    
    // load in the source
    const char* shaderSources[2] = {sShaderHeader, shaderSrc};
    GLint shaderLengths[2] =
    {
        (GLint)strlen(sShaderHeader),
        (GLint)strlen(shaderSrc)
    };
    glShaderSource( mShaderID, 2, (const GLchar**)shaderSources, shaderLengths );
    
    // compile it
    glCompileShader( mShaderID );
    GLint status = 0;
    glGetShaderiv( mShaderID, GL_COMPILE_STATUS, &status );
    if ( 0 == status )
    {
        GLint len;
        glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &len);
        if(len > 0) 
        {
            char* str = new char[len];
            glGetShaderInfoLog(mShaderID, len, nullptr, str);
            ERROR_OUT("Vertex shader error: ");
            ERROR_OUT(str << std::endl);
            delete [] str;
        }
        delete [] shaderSrc;
        return false;
    }

    delete [] shaderSrc;
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::CreateFromString()
//-------------------------------------------------------------------------------
// Create a shader from a string
//-------------------------------------------------------------------------------
bool
IvVertexShaderOGL::CreateFromString( const char* string )
{
    // allocate the shader id
    mShaderID = glCreateShader( GL_VERTEX_SHADER );
    if ( mShaderID == 0 )
        return false;
    
    // load in the source
    const char* shaderSources[2] = {sShaderHeader, string};
    GLint shaderLengths[2] =
    {
        (GLint)strlen(sShaderHeader),
        (GLint)strlen(string)
    };
    glShaderSource( mShaderID, 2, (const GLchar**)shaderSources, shaderLengths );
    
    // compile it
    glCompileShader( mShaderID );
    GLint status = 0;
    glGetShaderiv( mShaderID, GL_COMPILE_STATUS, &status );
    if ( 0 == status )
    {
        GLint len;
        glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &len);
        if(len > 0) 
        {
            char* str = new char[len];
            glGetShaderInfoLog(mShaderID, len, nullptr, str);
            DEBUG_OUT("Vertex shader error: ");
            DEBUG_OUT(str << std::endl);
        }
        return false;
    }
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::CreateDefault()
//-------------------------------------------------------------------------------
// Create a default shader
//-------------------------------------------------------------------------------
bool
IvVertexShaderOGL::CreateDefault( IvVertexFormat format )
{
    return CreateFromString(sDefaultVertexShader[format]);
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvVertexShaderOGL::Destroy()
{
    glDeleteShader(mShaderID);
    mShaderID = 0;
}
