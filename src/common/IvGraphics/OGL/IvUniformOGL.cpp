//===============================================================================
// @ IvUniformOGL.cpp
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

#include "IvUniformOGL.h"
#include "IvShaderProgramOGL.h"
#include <IvRenderer.h>
#include "IvTextureOGL.h"
#include <IvVector4.h>
#include <IvMatrix44.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvUniformOGL::IvUniformOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvUniformOGL::IvUniformOGL(IvUniformType type, unsigned int count, 
                           IvShaderProgramOGL* shader, unsigned int shaderIndex, 
                           unsigned int stage)
    : IvUniform( type, count )
    , mShader( shader )
    , mShaderIndex( shaderIndex)
    , mTextureStage( 0 )
{
    switch (mType)
    {
        case kFloatUniform:
            mValue.mFloat = new float[mCount];
            break;
        case kFloat3Uniform:
            mValue.mVector3 = new IvVector3[mCount];
            break;
        case kFloat4Uniform:
            mValue.mVector4 = new IvVector4[mCount];
            break;
        case kFloatMatrix44Uniform:
            mValue.mMatrix44 = new IvMatrix44[mCount];
            break;
        case kTextureUniform:
            mValue.mTexture = 0;
            mTextureStage = stage;
            break;
    };
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::~IvUniformOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvUniformOGL::~IvUniformOGL()
{
    switch (mType)
    {
        case kFloatUniform:
            delete [] mValue.mFloat;
            break;
        case kFloat3Uniform:
            delete [] mValue.mVector3;
            break;
        case kFloat4Uniform:
            delete [] mValue.mVector4;
            break;
        case kFloatMatrix44Uniform:
            delete [] mValue.mMatrix44;
            break;
        case kTextureUniform:
            break;
    };
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::SetValue()
//-------------------------------------------------------------------------------
// Float set
//-------------------------------------------------------------------------------
void IvUniformOGL::SetValue( float value, unsigned int index)
{
    if (mType != kFloatUniform)
        return;

    mValue.mFloat[index] = value;

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        glUniform1fv(mShaderIndex, mCount, mValue.mFloat);
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::SetValue()
//-------------------------------------------------------------------------------
// Float vec3 set
//-------------------------------------------------------------------------------
void IvUniformOGL::SetValue( const IvVector3& value, unsigned int index )
{
    if (mType != kFloat3Uniform)
        return;
    
    mValue.mVector3[index] = value;
    
    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        glUniform3fv(mShaderIndex, mCount, (GLfloat*)mValue.mVector3);
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::SetValue()
//-------------------------------------------------------------------------------
// Float vec4 set
//-------------------------------------------------------------------------------
void IvUniformOGL::SetValue( const IvVector4& value, unsigned int index )
{
    if (mType != kFloat4Uniform)
        return;

    mValue.mVector4[index] = value;

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        glUniform4fv(mShaderIndex, mCount, (GLfloat*)mValue.mVector4);
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::SetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 set
//-------------------------------------------------------------------------------
void IvUniformOGL::SetValue( const IvMatrix44& value, unsigned int index )
{
    if (mType != kFloatMatrix44Uniform)
        return;

    mValue.mMatrix44[index] = value;

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        glUniformMatrix4fv( mShaderIndex, mCount, GL_FALSE, (GLfloat*)mValue.mMatrix44);
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::SetValue()
//-------------------------------------------------------------------------------
// Texture set
//-------------------------------------------------------------------------------
void IvUniformOGL::SetValue( IvTexture* value )
{
    if (mType != kTextureUniform)
        return;

    mValue.mTexture = static_cast<IvTextureOGL*>(value);

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        glUniform1iv( mShaderIndex, 1, &mTextureStage);
        mValue.mTexture->MakeActive(mTextureStage);
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}
    
//-------------------------------------------------------------------------------
// @ IvUniformOGL::GetValue()
//-------------------------------------------------------------------------------
// Float get
//-------------------------------------------------------------------------------
bool IvUniformOGL::GetValue( float& value, unsigned int index) const
{
    if (mType != kFloatUniform)
        return false;

    value = mValue.mFloat[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::GetValue()
//-------------------------------------------------------------------------------
// Float vec4 get
//-------------------------------------------------------------------------------
bool IvUniformOGL::GetValue( IvVector3& value, unsigned int index ) const
{
    if (mType != kFloat3Uniform)
        return false;
    
    value = mValue.mVector3[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::GetValue()
//-------------------------------------------------------------------------------
// Float vec4 get
//-------------------------------------------------------------------------------
bool IvUniformOGL::GetValue( IvVector4& value, unsigned int index ) const
{
    if (mType != kFloat4Uniform)
        return false;

    value = mValue.mVector4[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::GetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 get
//-------------------------------------------------------------------------------
bool IvUniformOGL::GetValue( IvMatrix44& value, unsigned int index ) const
{
    if (mType != kFloatMatrix44Uniform)
        return false;

    value = mValue.mMatrix44[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformOGL::GetValue()
//-------------------------------------------------------------------------------
// Texture get
//-------------------------------------------------------------------------------
bool IvUniformOGL::GetValue( IvTexture*& value ) const
{
    if (mType != kTextureUniform)
        return false;

    value = mValue.mTexture;
    return true;
}

void IvUniformOGL::Update()
{
    if (mShader != IvRenderer::mRenderer->GetShaderProgram())
        return;

    if ( mType == kTextureUniform )
    {
        mValue.mTexture->MakeActive(mTextureStage);
    }

    if (!mNeedsUpdate)
        return;

    switch (mType)
    {
        case kFloatUniform:
            glUniform1fv(mShaderIndex, mCount, mValue.mFloat);
            break;
        case kFloat3Uniform:
            glUniform3fv(mShaderIndex, mCount, (GLfloat*)mValue.mVector3);
            break;
        case kFloat4Uniform:
            glUniform4fv(mShaderIndex, mCount, (GLfloat*)mValue.mVector4);
            break;
        case kFloatMatrix44Uniform:
            glUniformMatrix4fv(mShaderIndex, mCount, GL_FALSE, (GLfloat*)mValue.mMatrix44);
            break;
        case kTextureUniform:
            glUniform1iv(mShaderIndex, 1, &mTextureStage);
            break;
    };
    
    mNeedsUpdate = false;

}
