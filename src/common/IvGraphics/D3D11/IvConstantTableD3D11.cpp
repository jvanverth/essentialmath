//===============================================================================
// @ IvConstantTableD3D11.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2014  James M. Van Verth & Lars M. Bishop
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <d3d11.h>
#include <d3dcompiler.h>

#include "IvConstantTableD3D11.h"
#include "IvAssert.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvConstantTableD3D11::Create()
//-------------------------------------------------------------------------------
// Get the constant table for this shader
//-------------------------------------------------------------------------------
IvConstantTableD3D11*
IvConstantTableD3D11::Create(ID3D11Device* device, ID3DBlob* code)
{
	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(code->GetBufferPointer(), code->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&pReflector);

	// first get constant table
	IvConstantTableD3D11* table = new IvConstantTableD3D11();

	ID3D11ShaderReflectionConstantBuffer* globalConstants =
		pReflector->GetConstantBufferByName("$Globals");
	D3D11_SHADER_BUFFER_DESC constantBufferDesc = { 0 };
	HRESULT hr = globalConstants->GetDesc(&constantBufferDesc);
	if (FAILED(hr))
	{
		// no constants in this shader
		goto get_textures;
	}

	table->mBacking = new char[constantBufferDesc.Size];
	table->mBackingSize = constantBufferDesc.Size;
	if (NULL == table->mBacking)
	{
		delete table;
		pReflector->Release();
		return NULL;
	}

	for (unsigned int i = 0; i < constantBufferDesc.Variables; ++i)
	{
		ID3D11ShaderReflectionVariable* var = globalConstants->GetVariableByIndex(i);
		D3D11_SHADER_VARIABLE_DESC varDesc;
		if (FAILED(var->GetDesc(&varDesc)))
		{
			delete table;
			pReflector->Release();
			return NULL;
		}
		ID3D11ShaderReflectionType* type = var->GetType();
		D3D11_SHADER_TYPE_DESC typeDesc;
		if (FAILED(type->GetDesc(&typeDesc)))
		{
			delete table;
			pReflector->Release();
			return NULL;
		}

		IvConstantDesc constantDesc;
		constantDesc.mOffset = table->mBacking + varDesc.StartOffset;
		if (typeDesc.Class == D3D_SVC_SCALAR && typeDesc.Type == D3D_SVT_FLOAT)
		{
			constantDesc.mType = IvUniformType::kFloatUniform;
		}
		else if (typeDesc.Class == D3D_SVC_VECTOR && typeDesc.Type == D3D_SVT_FLOAT
			&& typeDesc.Columns == 3)
		{
			constantDesc.mType = IvUniformType::kFloat3Uniform;
		}
		else if (typeDesc.Class == D3D_SVC_VECTOR && typeDesc.Type == D3D_SVT_FLOAT
			&& typeDesc.Columns == 4)
		{
			constantDesc.mType = IvUniformType::kFloat4Uniform;
		}
		else if ((typeDesc.Class == D3D_SVC_MATRIX_ROWS || typeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
			&& typeDesc.Type == D3D_SVT_FLOAT
			&& typeDesc.Rows == 4 && typeDesc.Columns == 4)
		{
			constantDesc.mType = IvUniformType::kFloatMatrix44Uniform;
		}
		else
		{
			// unsupported uniform type
			delete table;
			pReflector->Release();
			return NULL;
		}
		constantDesc.mCount = typeDesc.Elements != 0 ? typeDesc.Elements : 1;

		table->mConstants[varDesc.Name] = constantDesc;
	}

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = constantBufferDesc.Size;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&cbDesc, NULL, &table->mBuffer)))
	{
		delete table;
		pReflector->Release();
		return NULL;
	}

get_textures:
	// now get any textures and samplers
	D3D11_SHADER_DESC shaderDesc = { 0 };
	hr = pReflector->GetDesc(&shaderDesc);
	if (FAILED(hr))
	{
		pReflector->Release();
		return table;
	}
	int resourceCount = shaderDesc.BoundResources;

	for (int i = 0; i < resourceCount; ++i) 
	{
		D3D11_SHADER_INPUT_BIND_DESC bindingDesc = { 0 };
		pReflector->GetResourceBindingDesc(i, &bindingDesc);
		if (bindingDesc.Type == D3D_SIT_TEXTURE && bindingDesc.ReturnType == D3D_RETURN_TYPE_FLOAT
			&& bindingDesc.Dimension == D3D_SRV_DIMENSION_TEXTURE2D && bindingDesc.NumSamples == -1
			&& bindingDesc.BindCount == 1)
		{
			IvConstantDesc& constantDesc = table->mConstants[bindingDesc.Name];
			constantDesc.mType = IvUniformType::kTextureUniform;
			constantDesc.mTextureSlot = bindingDesc.BindPoint;
		}
		else if (bindingDesc.Type == D3D_SIT_SAMPLER && bindingDesc.BindCount == 1)
		{
			IvConstantDesc& constantDesc = table->mConstants[bindingDesc.Name];
			constantDesc.mType = IvUniformType::kTextureUniform;
			constantDesc.mSamplerSlot = bindingDesc.BindPoint;
		}
		else if (bindingDesc.Type == D3D_SIT_CBUFFER && !_strnicmp(bindingDesc.Name, "$Globals", 8))
		{
			// ignore this, we handled it above
		}
		else
		{
			// unsupported resource type
			delete table;
			pReflector->Release();
			return NULL;
		}
	}

	pReflector->Release();

	return table;
}

//-------------------------------------------------------------------------------
// @ IvConstantTableD3D11::Destroy()
//-------------------------------------------------------------------------------
// Destroy the constant table
//-------------------------------------------------------------------------------
void IvConstantTableD3D11::Destroy(IvConstantTableD3D11* table)
{
	ASSERT(table->mRefCount >= 0);
	--table->mRefCount;
	if (table->mRefCount == 0)
	{
		delete table;
	}
}

//-------------------------------------------------------------------------------
// @ IvConstantTableD3D11::GetConstantDesc()
//-------------------------------------------------------------------------------
// Get the constant descriptor that matches this name
//-------------------------------------------------------------------------------
bool IvConstantTableD3D11::GetConstantDesc(const char* name, IvConstantDesc* constantDesc)
{
	if (mConstants.find(name) != mConstants.end())
	{
		memcpy(constantDesc, &mConstants[name], sizeof(IvConstantDesc));
		if (constantDesc->mType == kTextureUniform)
		{
			ASSERT(constantDesc->mSamplerSlot == constantDesc->mTextureSlot);
		}
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------
// @ IvConstantTableD3D11::MakeActiveVS()
//-------------------------------------------------------------------------------
// Make this constant buffer active for vertex shader
//-------------------------------------------------------------------------------
bool IvConstantTableD3D11::MakeActiveVS(ID3D11DeviceContext* context)
{
	if (mBuffer == 0 || NULL == mBacking)
		return false;

	if (mDirty)
	{
		// Lock the constant buffer so it can be written to.
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = context->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		// Copy the data
		memcpy(mappedResource.pData, mBacking, mBackingSize);

		// Unlock the constant buffer.
		context->Unmap(mBuffer, 0);

		mDirty = false;
	}

	context->VSSetConstantBuffers(0, 1, &mBuffer);
	return true;
}

//-------------------------------------------------------------------------------
// @ IvConstantTableD3D11::MakeActivePS()
//-------------------------------------------------------------------------------
// Make this constant buffer active for pixel shader
//-------------------------------------------------------------------------------
bool IvConstantTableD3D11::MakeActivePS(ID3D11DeviceContext* context)
{
	if (mBuffer == 0 || NULL == mBacking)
		return false;

	if (mDirty)
	{
		// Lock the constant buffer so it can be written to.
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = context->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		// Copy the data
		memcpy(mappedResource.pData, mBacking, mBackingSize);

		// Unlock the constant buffer.
		context->Unmap(mBuffer, 0);

		mDirty = false;
	}

	context->PSSetConstantBuffers(0, 1, &mBuffer);
	return true;
}
