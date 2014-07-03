//===============================================================================
// @ IvUniformDX11.cpp
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

#include "IvUniformDX11.h"
#include "IvConstantTableDX11.h"
#include "IvShaderProgramDX11.h"
#include "IvRendererDX11.h"
#include "IvTextureDX11.h"
#include <IvVector4.h>
#include <IvMatrix44.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvUniformDX11::IvUniformDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvUniformDX11::IvUniformDX11(IvUniformType type, void* offset, unsigned int count,
							 IvConstantTableDX11* constantTable,
							 IvShaderProgramDX11* shader)
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
        case kFloat4Uniform:
            mValue.mVector4 = new IvVector4[mCount];
            break;
        case kFloatMatrix44Uniform:
            mValue.mMatrix44 = new IvMatrix44[mCount];
            break;
        case kTextureUniform:
            mValue.mTexture = 0;
            break;
    };
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::~IvUniformDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvUniformDX11::~IvUniformDX11()
{
    switch (mType)
    {
        case kFloatUniform:
            delete [] mValue.mFloat;
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
// @ IvUniformDX11::SetValue()
//-------------------------------------------------------------------------------
// Float set
//-------------------------------------------------------------------------------
void IvUniformDX11::SetValue( float value, unsigned int index)
{
    if (mType != kFloatUniform)
        return;

    mValue.mFloat[index] = value;

	if (mShader == IvRenderer::mRenderer->GetShaderProgram())
	{
		memcpy(mOffset, mValue.mFloat, mCount*sizeof(float));
		mConstantTable->MarkDirty();
	}
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::SetValue()
//-------------------------------------------------------------------------------
// Float vec4 set
//-------------------------------------------------------------------------------
void IvUniformDX11::SetValue( const IvVector4& value, unsigned int index )
{
    if (mType != kFloat4Uniform)
        return;

    mValue.mVector4[index] = value;

	if (mShader == IvRenderer::mRenderer->GetShaderProgram())
	{
		memcpy(mOffset, mValue.mVector4, mCount*sizeof(IvVector4));
		mConstantTable->MarkDirty();
	}
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::SetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 set
//-------------------------------------------------------------------------------
void IvUniformDX11::SetValue( const IvMatrix44& value, unsigned int index )
{
    if (mType != kFloatMatrix44Uniform)
        return;

    mValue.mMatrix44[index] = value;

	if (mShader == IvRenderer::mRenderer->GetShaderProgram())
	{
		memcpy(mOffset, mValue.mMatrix44, mCount*sizeof(IvMatrix44));
		mConstantTable->MarkDirty();
	}
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::SetValue()
//-------------------------------------------------------------------------------
// Texture set
//-------------------------------------------------------------------------------
void IvUniformDX11::SetValue( IvTexture* value )
{
/***   if (mType != kTextureUniform)
        return;

    mValue.mTexture = static_cast<IvTextureDX11*>(value);

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        unsigned int stage = mConstantTable->GetSamplerIndex(mUniform);
        mValue.mTexture->MakeActive(stage, static_cast<IvRendererDX11*>(IvRenderer::mRenderer)->GetDevice());
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }***/
}
    
//-------------------------------------------------------------------------------
// @ IvUniformDX11::GetValue()
//-------------------------------------------------------------------------------
// Float get
//-------------------------------------------------------------------------------
bool IvUniformDX11::GetValue( float& value, unsigned int index) const
{
    if (mType != kFloatUniform)
        return false;

    value = mValue.mFloat[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::GetValue()
//-------------------------------------------------------------------------------
// Float vec4 get
//-------------------------------------------------------------------------------
bool IvUniformDX11::GetValue( IvVector4& value, unsigned int index ) const
{
    if (mType != kFloat4Uniform)
        return false;

    value = mValue.mVector4[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::GetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 get
//-------------------------------------------------------------------------------
bool IvUniformDX11::GetValue( IvMatrix44& value, unsigned int index ) const
{
    if (mType != kFloatMatrix44Uniform)
        return false;

    value = mValue.mMatrix44[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformDX11::GetValue()
//-------------------------------------------------------------------------------
// Texture get
//-------------------------------------------------------------------------------
bool IvUniformDX11::GetValue( IvTexture*& value ) const
{
	if (mType != kTextureUniform)
        return false;

	value = mValue.mTexture;
	return true;
}


//-------------------------------------------------------------------------------
// @ IvUniformDX11::Update()
//-------------------------------------------------------------------------------
// Update current uniform values if needed
//-------------------------------------------------------------------------------
void IvUniformDX11::Update()
{
/***    if (mShader != IvRenderer::mRenderer->GetShaderProgram())
        return;

	if ( mType == kTextureUniform )
	{
        unsigned int stage = mConstantTable->GetSamplerIndex(mUniform);
        mValue.mTexture->MakeActive(stage, static_cast<IvRendererDX11*>(IvRenderer::mRenderer)->GetDevice());
		return;
	}

    if (!mNeedsUpdate)
        return;

    switch (mType)
    {
        case kFloatUniform:
            (void) mConstantTable->SetFloatArray( static_cast<IvRendererDX11*>(IvRenderer::mRenderer)->GetDevice(),
											      mUniform, (const float*)mValue.mFloat, mCount );
            break;
        case kFloat4Uniform:
            (void) mConstantTable->SetVectorArray( static_cast<IvRendererDX11*>(IvRenderer::mRenderer)->GetDevice(), 
				                                   mUniform, (const D3DXVECTOR4*)mValue.mVector4, mCount );
            break;
        case kFloatMatrix44Uniform:
            (void) mConstantTable->SetMatrixArray( static_cast<IvRendererDX11*>(IvRenderer::mRenderer)->GetDevice(), 
				                                   mUniform, (const D3DXMATRIX*)mValue.mMatrix44, mCount );
            break;
    };
***/
}
