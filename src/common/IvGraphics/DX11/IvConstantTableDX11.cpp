//===============================================================================
// @ IvConstantTableDX11.cpp
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

#include "IvConstantTableDX11.h"
#include "IvAssert.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvConstantTableDX11::Create()
//-------------------------------------------------------------------------------
// Get the constant table for this shader
//-------------------------------------------------------------------------------
IvConstantTableDX11*
IvConstantTableDX11::Create(ID3D11Device* device, ID3DBlob* code)
{
	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(code->GetBufferPointer(), code->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&pReflector);
/*	D3D11_SHADER_DESC shaderDesc = { 0 };
	HRESULT hr = pReflector->GetDesc(&shaderDesc);
	if (FAILED(hr))
	{
		pReflector->Release();
		return NULL;
	}*/

	IvConstantTableDX11* table = new IvConstantTableDX11();

	ID3D11ShaderReflectionConstantBuffer* globalConstants =
		pReflector->GetConstantBufferByName("$Globals");
	D3D11_SHADER_BUFFER_DESC constantBufferDesc = { 0 };
	HRESULT hr = globalConstants->GetDesc(&constantBufferDesc);
	if (FAILED(hr))
	{
		// no constants in this shader
		pReflector->Release();

		return table;
	}

	table->mBacking = new char[constantBufferDesc.Size];
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
			&& typeDesc.Columns == 4)  //*** this is a guess
		{
			constantDesc.mType = IvUniformType::kFloat4Uniform;
		}
		else if ((typeDesc.Class == D3D_SVC_MATRIX_ROWS || typeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
			&& typeDesc.Type == D3D_SVT_FLOAT
			&& typeDesc.Rows == 4 && typeDesc.Columns == 4) //*** this is a guess
		{
			constantDesc.mType = IvUniformType::kFloatMatrix44Uniform;
		}
		else if (typeDesc.Type == D3D_SVT_TEXTURE2D) //*** this is a guess
		{
			constantDesc.mType = IvUniformType::kTextureUniform;
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

	pReflector->Release();

	return table;
}

//-------------------------------------------------------------------------------
// @ IvConstantTableDX11::Destroy()
//-------------------------------------------------------------------------------
// Destroy the constant table
//-------------------------------------------------------------------------------
void IvConstantTableDX11::Destroy(IvConstantTableDX11* table)
{
	ASSERT(table->mRefCount >= 0);
	--table->mRefCount;
	if (table->mRefCount == 0)
	{
		delete table;
	}
}

//-------------------------------------------------------------------------------
// @ IvConstantTableDX11::GetConstantDesc()
//-------------------------------------------------------------------------------
// Get the constant descriptor that matches this name
//-------------------------------------------------------------------------------
bool IvConstantTableDX11::GetConstantDesc(const char* name, IvConstantDesc* constantDesc)
{
	if (mConstants.find(name) != mConstants.end())
	{
		memcpy(constantDesc, &mConstants[name], sizeof(IvConstantDesc));
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------
// @ IvConstantTableDX11::MakeActive()
//-------------------------------------------------------------------------------
// Make this constant buffer active
//-------------------------------------------------------------------------------
bool IvConstantTableDX11::MakeActive(ID3D11DeviceContext* context)
{
	return false;
}
