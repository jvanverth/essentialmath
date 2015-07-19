//===============================================================================
// @ IvShaderProgramOGL.cpp
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

#include "IvShaderProgramOGL.h"
#include "IvFragmentShaderOGL.h"
#include "IvUniformOGL.h"
#include "IvVertexShaderOGL.h"
#include "IvDebugger.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvShaderProgramOGL::IvShaderProgramOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvShaderProgramOGL::IvShaderProgramOGL() : IvShaderProgram(), mProgramID(0), mNextTextureStage(0)
{
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramOGL::~IvShaderProgramOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvShaderProgramOGL::~IvShaderProgramOGL()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramOGL::CreateFromFile()
//-------------------------------------------------------------------------------
// Create a shader from a file
//-------------------------------------------------------------------------------
bool
IvShaderProgramOGL::Create( GLuint vertexShaderID, GLuint fragmentShaderID )
{
    // check for valid inputs
    if ( 0 == vertexShaderID || 0 == fragmentShaderID )
        return false;
    
    // allocate the shader id
    mProgramID = glCreateProgram();
    if ( mProgramID == 0 )
        return false;
    
    // attach the shaders
    glAttachShader( mProgramID, vertexShaderID );
    glAttachShader( mProgramID, fragmentShaderID );
    
    // link it
    glLinkProgram( mProgramID );
    GLint status = 0;
    glGetProgramiv( mProgramID, GL_LINK_STATUS, &status );
    if ( 0 == status )
        return false;
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramOGL::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvShaderProgramOGL::Destroy()
{
    if ( mProgramID != 0 )
    {
        glDeleteProgram( mProgramID );
    }
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramOGL::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active program
//-------------------------------------------------------------------------------
bool
IvShaderProgramOGL::MakeActive()
{
    if ( mProgramID == 0 )
        return false;

    glUseProgram( mProgramID );

    std::map<std::string, IvUniformOGL*>::iterator iter = mUniforms.begin();
   
    while (iter != mUniforms.end())
    {
        iter->second->Update();
        
        iter++;
    }

    return true;
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramOGL::GetUniform()
//-------------------------------------------------------------------------------
// Queries, returns and caches a shader uniform
//-------------------------------------------------------------------------------
IvUniform*
IvShaderProgramOGL::GetUniform(char const* name)
{
    if (!name)
    {
        return NULL;
    }

    // Did we already query the uniform?
    if (mUniforms.find(name) != mUniforms.end())
    {
        return mUniforms[name];
    }

    // First, query the location
    GLint location = glGetUniformLocation(mProgramID, name);
    if (location == -1)
        return NULL;
    
    // now find it in the list of uniforms to get type and size
    GLint numUniforms = 0;
    glGetProgramiv(	mProgramID, GL_ACTIVE_UNIFORMS, &numUniforms );
    GLint uniformMaxLength = 0;
    glGetProgramiv(	mProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength );
    GLint count = -1;
    GLenum type = 0;
    GLchar* uniformName = new GLchar[uniformMaxLength];
    bool found = false;
    int maxlen = strlen(name);
    for ( GLint i = 0; i < numUniforms; ++i )
    {
        GLsizei length;
        glGetActiveUniform(	mProgramID, i, uniformMaxLength, &length, &count, &type, uniformName);              
        if (strncmp(uniformName, name, maxlen) == 0)
        {
            found = true;
            break;
        }
    }
    delete [] uniformName;

    // not found
    if ( !found || count < 0 )
        return NULL;
    
    IvUniformType ivType;
    switch (type)
    {
        case GL_FLOAT:
            ivType = kFloatUniform;
            break;
        case GL_FLOAT_VEC3:
            ivType = kFloat3Uniform;
            break;
        case GL_FLOAT_VEC4:
            ivType = kFloat4Uniform;
            break;
        case GL_FLOAT_MAT4:
            ivType = kFloatMatrix44Uniform;
            break;
        case GL_SAMPLER_2D:
            ivType = kTextureUniform;
            break;
        default:
            return NULL;
    };

	IvUniformOGL* uniform = new IvUniformOGL(ivType, (unsigned int)count, this, location, 
											 ivType == kTextureUniform?mNextTextureStage++:0);
    mUniforms[name] = uniform;

    return uniform;
}
