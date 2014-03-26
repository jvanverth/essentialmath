//===============================================================================
// @ IvVertexShaderOGL.cpp
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

#include "IvVertexShaderOGL.h"
#include "IvDebugger.h"
#include <string>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static char const* sDefaultVertexShader[kVertexFormatCount] = {0};

static const char sShaderCPFormat[] = 
"varying vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"
"    color = gl_Color;\n"
"}\n";

static const char sShaderNPFormat[] = 
"uniform mat4 IvNormalMatrix;\n"
"uniform vec4 IvLightDirection;\n"
"uniform vec4 IvLightAmbient;\n"
"uniform vec4 IvLightDiffuse;\n"
"uniform vec4 IvDiffuseColor;\n"
"varying vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"
"    vec4 transNormal = normalize(IvNormalMatrix * vec4(gl_Normal,0.0));\n"
"    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);\n"
"    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    color = IvDiffuseColor*lightValue;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"uniform mat4 IvNormalMatrix;\n"
"uniform vec4 IvLightDirection;\n"
"uniform vec4 IvLightAmbient;\n"
"uniform vec4 IvLightDiffuse;\n"
"varying vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"
"    vec4 transNormal = normalize(IvNormalMatrix * vec4(gl_Normal,0.0));\n"
"    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);\n"
"    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    color = gl_Color*lightValue;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"
"    color = gl_Color;\n"
"    uv = gl_MultiTexCoord0.xy;\n"
"}\n";

static const char sShaderTNPFormat[] = 
"uniform mat4 IvNormalMatrix;\n"
"uniform vec4 IvLightDirection;\n"
"uniform vec4 IvLightAmbient;\n"
"uniform vec4 IvLightDiffuse;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main()\n"\
"{\n"
"    gl_Position = ftransform();\n"
"    vec4 transNormal = normalize(IvNormalMatrix * vec4(gl_Normal,0.0));\n"
"    float ndotVP = clamp(dot(transNormal,IvLightDirection), 0.0, 1.0);\n"
"    vec4 lightValue = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    color = lightValue;\n"
"    uv = gl_MultiTexCoord0.xy;\n"
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
    Destroy();
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
        return false;

    fseek(fp, 0, SEEK_END);
    unsigned int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* shaderSrc = new char[length+1];

    length = fread(shaderSrc, 1, length, fp);
    shaderSrc[length] = 0;
	fclose(fp);

    // allocate the shader id
    mShaderID = glCreateShader( GL_VERTEX_SHADER );
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
            ERROR_OUT("Vertex shader error: ");
            ERROR_OUT(str << std::endl);
        }
        return false;
    }

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
    GLint length = strlen(string);
    glShaderSource( mShaderID, 1, &string, &length );
    
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
    if ( mShaderID != 0 )
        glDeleteShader( mShaderID );
}
