//===============================================================================
// @ IvUniformD3D11.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2013  James M. Van Verth & Lars M. Bishop
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvUniformD3D11.h"
#include "IvAssert.h"
#include "IvConstantTableD3D11.h"
#include "IvShaderProgramD3D11.h"
#include "IvRendererD3D11.h"
#include "IvTextureD3D11.h"
#include <IvVector4.h>
#include <IvMatrix44.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::IvUniformD3D11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvUniformD3D11::IvUniformD3D11(IvUniformType type, unsigned int count, 
                             void* offset,
                             IvConstantTableD3D11* constantTable,
                             IvShaderProgramD3D11* shader)
    : IvUniform( type, count )
    , mShader( shader )
    , mOffset( offset )
    , mConstantTable( constantTable )
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
            ASSERT(false);
            mValue.mTexture = 0;
            break;
    };
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::IvUniformD3D11()
//-------------------------------------------------------------------------------
// Texture uniform constructor
//-------------------------------------------------------------------------------
IvUniformD3D11::IvUniformD3D11(int textureUnit, int samplerUnit,
                             IvShaderProgramD3D11* shader)
    : IvUniform(kTextureUniform, 1)
    , mShader(shader)
    , mTextureUnit(textureUnit)
    , mSamplerUnit(samplerUnit)
{
    mValue.mTexture = 0;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::~IvUniformD3D11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvUniformD3D11::~IvUniformD3D11()
{
    switch (mType)
    {
        case kFloatUniform:
            delete [] mValue.mFloat;
            break;
        case kFloat3Uniform:
            delete[] mValue.mVector3;
            break;
        case kFloat4Uniform:
            delete [] mValue.mVector4;
            break;
        case kFloatMatrix44Uniform:
            delete [] mValue.mMatrix44;
            break;
    };
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::SetValue()
//-------------------------------------------------------------------------------
// Float set
//-------------------------------------------------------------------------------
void IvUniformD3D11::SetValue( float value, unsigned int index)
{
    if (mType != kFloatUniform)
        return;

    mValue.mFloat[index] = value;

    memcpy(mOffset, mValue.mFloat, mCount*sizeof(float));
    mConstantTable->MarkDirty();
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::SetValue()
//-------------------------------------------------------------------------------
// Float vec3 set
//-------------------------------------------------------------------------------
void IvUniformD3D11::SetValue(const IvVector3& value, unsigned int index)
{
    if (mType != kFloat3Uniform)
        return;

    mValue.mVector3[index] = value;

    // float3 array is packed as float4 array
    char* offset = reinterpret_cast<char*>(mOffset);
    IvVector3* currVec3 = mValue.mVector3;
    for (unsigned int i = 0; i < mCount; ++i)
    {
        memcpy(offset, currVec3, sizeof(IvVector3));
        ++currVec3;
        offset += 4*sizeof(float);
    }
    mConstantTable->MarkDirty();
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::SetValue()
//-------------------------------------------------------------------------------
// Float vec4 set
//-------------------------------------------------------------------------------
void IvUniformD3D11::SetValue( const IvVector4& value, unsigned int index )
{
    if (mType != kFloat4Uniform)
        return;

    mValue.mVector4[index] = value;

    memcpy(mOffset, mValue.mVector4, mCount*sizeof(IvVector4));
    mConstantTable->MarkDirty();
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::SetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 set
//-------------------------------------------------------------------------------
void IvUniformD3D11::SetValue( const IvMatrix44& value, unsigned int index )
{
    if (mType != kFloatMatrix44Uniform)
        return;

    mValue.mMatrix44[index] = value;

    memcpy(mOffset, mValue.mMatrix44, mCount*sizeof(IvMatrix44));
    mConstantTable->MarkDirty();
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::SetValue()
//-------------------------------------------------------------------------------
// Texture set
//-------------------------------------------------------------------------------
void IvUniformD3D11::SetValue( IvTexture* value )
{
    if (mType != kTextureUniform)
    {
        return;
    }

    mValue.mTexture = static_cast<IvTextureD3D11*>(value);

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        mValue.mTexture->MakeActive(mTextureUnit, mSamplerUnit, static_cast<IvRendererD3D11*>(IvRenderer::mRenderer)->GetDevice());
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}
    
//-------------------------------------------------------------------------------
// @ IvUniformD3D11::GetValue()
//-------------------------------------------------------------------------------
// Float get
//-------------------------------------------------------------------------------
bool IvUniformD3D11::GetValue( float& value, unsigned int index) const
{
    if (mType != kFloatUniform)
        return false;

    value = mValue.mFloat[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::GetValue()
//-------------------------------------------------------------------------------
// Float vec3 get
//-------------------------------------------------------------------------------
bool IvUniformD3D11::GetValue(IvVector3& value, unsigned int index) const
{
    if (mType != kFloat3Uniform)
        return false;

    value = mValue.mVector3[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::GetValue()
//-------------------------------------------------------------------------------
// Float vec4 get
//-------------------------------------------------------------------------------
bool IvUniformD3D11::GetValue( IvVector4& value, unsigned int index ) const
{
    if (mType != kFloat4Uniform)
        return false;

    value = mValue.mVector4[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::GetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 get
//-------------------------------------------------------------------------------
bool IvUniformD3D11::GetValue( IvMatrix44& value, unsigned int index ) const
{
    if (mType != kFloatMatrix44Uniform)
        return false;

    value = mValue.mMatrix44[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D11::GetValue()
//-------------------------------------------------------------------------------
// Texture get
//-------------------------------------------------------------------------------
bool IvUniformD3D11::GetValue( IvTexture*& value ) const
{
    if (mType != kTextureUniform)
        return false;

    value = mValue.mTexture;
    return true;
}


//-------------------------------------------------------------------------------
// @ IvUniformD3D11::Update()
//-------------------------------------------------------------------------------
// Update current uniform values if needed
//-------------------------------------------------------------------------------
void IvUniformD3D11::Update()
{
    if (mShader != IvRenderer::mRenderer->GetShaderProgram())
    {
        return;
    }

    if ( mType == kTextureUniform )
    {
        mValue.mTexture->MakeActive(mTextureUnit, mSamplerUnit, static_cast<IvRendererD3D11*>(IvRenderer::mRenderer)->GetDevice());
        return;
    }

    if (!mNeedsUpdate)
        return;

}
