//===============================================================================
// @ IvUniformD3D9.cpp
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

#include "IvUniformD3D9.h"
#include "IvShaderProgramD3D9.h"
#include "IvRendererD3D9.h"
#include "IvTextureD3D9.h"
#include <IvVector4.h>
#include <IvMatrix44.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::IvUniformD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvUniformD3D9::IvUniformD3D9(IvUniformType type, unsigned int count,
							 D3DXHANDLE uniform, 
							 LPD3DXCONSTANTTABLE constantTable, 
							 IvShaderProgramD3D9* shader)
	: IvUniform( type, count )
	, mUniform( uniform )
	, mConstantTable( constantTable )
	, mShader( shader )
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
// @ IvUniformD3D9::~IvUniformD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvUniformD3D9::~IvUniformD3D9()
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
// @ IvUniformD3D9::SetValue()
//-------------------------------------------------------------------------------
// Float set
//-------------------------------------------------------------------------------
void IvUniformD3D9::SetValue( float value, unsigned int index)
{
    if (mType != kFloatUniform)
        return;

    mValue.mFloat[index] = value;

    if (mShader == IvRenderer::mRenderer->GetShaderProgram()
		&& (D3D_OK == mConstantTable->SetFloatArray( static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice(),
													mUniform, (const float*)mValue.mFloat, mCount )))
	{
		mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::SetValue()
//-------------------------------------------------------------------------------
// Float vec4 set
//-------------------------------------------------------------------------------
void IvUniformD3D9::SetValue( const IvVector4& value, unsigned int index )
{
    if (mType != kFloat4Uniform)
        return;

    mValue.mVector4[index] = value;

    if (mShader == IvRenderer::mRenderer->GetShaderProgram()
		&& (D3D_OK == mConstantTable->SetVectorArray( static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice(),
													  mUniform, (const D3DXVECTOR4*)mValue.mVector4, mCount )))
	{
		mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::SetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 set
//-------------------------------------------------------------------------------
void IvUniformD3D9::SetValue( const IvMatrix44& value, unsigned int index )
{
    if (mType != kFloatMatrix44Uniform)
        return;

    mValue.mMatrix44[index] = value;

	HRESULT result;
    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
	{
		if (D3D_OK == (result = mConstantTable->SetMatrixArray( static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice(),
													mUniform, (const D3DXMATRIX*)mValue.mMatrix44, mCount )))
		{
			mNeedsUpdate = false;
		}
		else
		{
			mNeedsUpdate = true;
		}
	}
	else
	{
		mNeedsUpdate = true;
	}
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::SetValue()
//-------------------------------------------------------------------------------
// Texture set
//-------------------------------------------------------------------------------
void IvUniformD3D9::SetValue( IvTexture* value )
{
   if (mType != kTextureUniform)
        return;

    mValue.mTexture = static_cast<IvTextureD3D9*>(value);

    if (mShader == IvRenderer::mRenderer->GetShaderProgram())
    {
        unsigned int stage = mConstantTable->GetSamplerIndex(mUniform);
        mValue.mTexture->MakeActive(stage, static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice());
        mNeedsUpdate = false;
    }
    else
    {
        mNeedsUpdate = true;
    }
}
    
//-------------------------------------------------------------------------------
// @ IvUniformD3D9::GetValue()
//-------------------------------------------------------------------------------
// Float get
//-------------------------------------------------------------------------------
bool IvUniformD3D9::GetValue( float& value, unsigned int index) const
{
    if (mType != kFloatUniform)
        return false;

    value = mValue.mFloat[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::GetValue()
//-------------------------------------------------------------------------------
// Float vec4 get
//-------------------------------------------------------------------------------
bool IvUniformD3D9::GetValue( IvVector4& value, unsigned int index ) const
{
    if (mType != kFloat4Uniform)
        return false;

    value = mValue.mVector4[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::GetValue()
//-------------------------------------------------------------------------------
// Float matrix 4x4 get
//-------------------------------------------------------------------------------
bool IvUniformD3D9::GetValue( IvMatrix44& value, unsigned int index ) const
{
    if (mType != kFloatMatrix44Uniform)
        return false;

    value = mValue.mMatrix44[index];
    return true;
}

//-------------------------------------------------------------------------------
// @ IvUniformD3D9::GetValue()
//-------------------------------------------------------------------------------
// Texture get
//-------------------------------------------------------------------------------
bool IvUniformD3D9::GetValue( IvTexture*& value ) const
{
	if (mType != kTextureUniform)
        return false;

	value = mValue.mTexture;
	return true;
}


//-------------------------------------------------------------------------------
// @ IvUniformD3D9::Update()
//-------------------------------------------------------------------------------
// Update current uniform values if needed
//-------------------------------------------------------------------------------
void IvUniformD3D9::Update()
{
    if (mShader != IvRenderer::mRenderer->GetShaderProgram())
        return;

	if ( mType == kTextureUniform )
	{
        unsigned int stage = mConstantTable->GetSamplerIndex(mUniform);
        mValue.mTexture->MakeActive(stage, static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice());
		return;
	}

    if (!mNeedsUpdate)
        return;

    switch (mType)
    {
        case kFloatUniform:
            (void) mConstantTable->SetFloatArray( static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice(),
											      mUniform, (const float*)mValue.mFloat, mCount );
            break;
        case kFloat4Uniform:
            (void) mConstantTable->SetVectorArray( static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice(), 
				                                   mUniform, (const D3DXVECTOR4*)mValue.mVector4, mCount );
            break;
        case kFloatMatrix44Uniform:
            (void) mConstantTable->SetMatrixArray( static_cast<IvRendererD3D9*>(IvRenderer::mRenderer)->GetDevice(), 
				                                   mUniform, (const D3DXMATRIX*)mValue.mMatrix44, mCount );
            break;
    };
}
