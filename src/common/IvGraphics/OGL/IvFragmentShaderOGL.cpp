//===============================================================================
// @ IvFragmentShaderOGL.cpp
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

#include "IvFragmentShaderOGL.h"
#include "IvAssert.h"
#include "IvDebugger.h"
#include <string>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static char const* sDefaultFragmentShader[kVertexFormatCount] = {0};

static const char sShaderPFormat[] =
"#version 150\n"
"uniform vec4 IvDiffuseColor;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = IvDiffuseColor;\n"
"}\n";

static const char sShaderCPFormat[] =
"#version 150\n"
"in vec4 color;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = color;\n"
"}\n";

static const char sShaderNPFormat[] = 
"#version 150\n"
"in vec4 color;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = color;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"#version 150\n"
"in vec4 color;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = color;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"#version 150\n"
"in sampler2D defaultTexture;\n"
"in vec2 uv;\n"
"in vec4 color;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = color*texture2D(defaultTexture, uv);\n"
"}\n";

static const char sShaderTNPFormat[] = 
"#version 150\n"
"in sampler2D defaultTexture;\n"
"in vec2 uv;\n"
"in vec4 color;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = color*texture2D(defaultTexture, uv);\n"
"}\n";

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvFragmentShaderOGL::IvFragmentShaderOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvFragmentShaderOGL::IvFragmentShaderOGL() : IvFragmentShader()
    , mShaderID(0)
{
    sDefaultFragmentShader[kPFormat] = sShaderPFormat;
    sDefaultFragmentShader[kCPFormat] = sShaderCPFormat;
    sDefaultFragmentShader[kNPFormat] = sShaderNPFormat;
    sDefaultFragmentShader[kCNPFormat] = sShaderCNPFormat;
    sDefaultFragmentShader[kTCPFormat] = sShaderTCPFormat;
    sDefaultFragmentShader[kTNPFormat] = sShaderTNPFormat;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderOGL::~IvFragmentShaderOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvFragmentShaderOGL::~IvFragmentShaderOGL()
{
    ASSERT(!mShaderID);
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderOGL::CreateFromFile()
//-------------------------------------------------------------------------------
// Load shader from a textfile
//-------------------------------------------------------------------------------
bool
IvFragmentShaderOGL::CreateFromFile( const char* filename )
{
    // Add the expected extension:
    std::string fullFilename = filename;

    fullFilename = fullFilename + std::string(".glslf");

    FILE* fp = fopen(fullFilename.c_str(), "r");

    if (!fp)
        return false;

    fseek(fp, 0, SEEK_END);
    unsigned int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* shaderSrc = new char[length+1];

    length = fread(shaderSrc, 1, length, fp);
    shaderSrc[length] = 0;

    // allocate the shader id
    mShaderID = glCreateShader( GL_FRAGMENT_SHADER );
    if ( mShaderID == 0 )
        return false;
    
    // load in the source
    GLint shaderLength = strlen(shaderSrc);
    glShaderSource( mShaderID, 1, (const GLchar**)&shaderSrc, &shaderLength );
    
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
            glGetShaderInfoLog(mShaderID, len, NULL, str);
            ERROR_OUT("Fragment shader error: ");
            ERROR_OUT(str << std::endl);
        }
        return false;
	}

    return true;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderOGL::CreateFromString()
//-------------------------------------------------------------------------------
// Create a shader from a string
//-------------------------------------------------------------------------------
bool
IvFragmentShaderOGL::CreateFromString( const char* string )
{
    // allocate the shader id
    mShaderID = glCreateShader( GL_FRAGMENT_SHADER );
    if ( mShaderID == 0 )
        return false;
    
    // load in the source
    GLint length = strlen(string);
    glShaderSource( mShaderID, 1, &string, &length );
    
    // compile it
    glCompileShader( mShaderID );
    GLint status = 0;
    glGetShaderiv( mShaderID, GL_COMPILE_STATUS, &status );
	if (status == 0)
	{
        GLint len;
        glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &len);
        if(len > 0) 
        {
            char* str = new char[len];
            glGetShaderInfoLog(mShaderID, len, NULL, str);
            DEBUG_OUT("Fragment shader error: ");
            DEBUG_OUT(str << std::endl);
        }
        return false;
	}
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderOGL::CreateDefault()
//-------------------------------------------------------------------------------
// Create default shader
//-------------------------------------------------------------------------------
bool 
IvFragmentShaderOGL::CreateDefault( IvVertexFormat format )
{
    return CreateFromString(sDefaultFragmentShader[format]);
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderOGL::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvFragmentShaderOGL::Destroy()
{
    glDeleteShader(mShaderID);
    mShaderID = 0;
}
