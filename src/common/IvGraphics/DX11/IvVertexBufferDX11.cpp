//===============================================================================
// @ IvVertexBufferDX11.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2014  James M. Van Verth & Lars M. Bishop. 
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVertexBufferDX11.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static ID3D11InputLayout * sInputLayout[kVertexFormatCount] = { 0 };

D3D11_INPUT_ELEMENT_DESC sCPFormatElements[] =
{
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC sNPFormatElements[] =
{
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC sCNPFormatElements[] =
{
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC sTCPFormatElements[] =
{
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC sTNPFormatElements[] =
{
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVertexBufferDX11::IvVertexBufferDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvVertexBufferDX11::IvVertexBufferDX11() : IvVertexBuffer(), mBufferPtr(0), mDataPtr(0)
{
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferDX11::~IvVertexBufferDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvVertexBufferDX11::~IvVertexBufferDX11()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferDX11::Create()
//-------------------------------------------------------------------------------
// Create the resources for the vertex buffer
//-------------------------------------------------------------------------------
bool
IvVertexBufferDX11::Create(IvVertexFormat format, unsigned int numVertices, void* data, ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = numVertices*kIvVFSize[format];
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	//*** replacement for D3DMANAGED?

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &initData, &mBufferPtr)))
	{
		mBufferPtr = 0;
		return false;
	}

	if (sInputLayout[format] == 0)
	{
		switch (format)
		{
		case kCPFormat:
			device->CreateInputLayout(sCPFormatElements, sizeof(sCPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC), NULL/*shaderBuffer*/, 0/*shaderBufferSize*/, &sInputLayout[format]);
			break;
		case kNPFormat:
			device->CreateInputLayout(sNPFormatElements, sizeof(sNPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC), NULL/*shaderBuffer*/, 0/*shaderBufferSize*/, &sInputLayout[format]);
			break;
		case kCNPFormat:
			device->CreateInputLayout(sCNPFormatElements, sizeof(sCNPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC), NULL/*shaderBuffer*/, 0/*shaderBufferSize*/, &sInputLayout[format]);
			break;
		case kTCPFormat:
			device->CreateInputLayout(sTCPFormatElements, sizeof(sTCPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC), NULL/*shaderBuffer*/, 0/*shaderBufferSize*/, &sInputLayout[format]);
			break;
		case kTNPFormat:
			device->CreateInputLayout(sTNPFormatElements, sizeof(sTNPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC), NULL/*shaderBuffer*/, 0/*shaderBufferSize*/, &sInputLayout[format]);
			break;
		}
	}
    else
    {
		sInputLayout[format]->AddRef();
    }

	mVertexFormat = format;
	mNumVertices = numVertices;

	return true;
}	

//-------------------------------------------------------------------------------
// @ IvVertexBufferDX11::Destroy()
//-------------------------------------------------------------------------------
// Destroy the resources for the vertex buffer
//-------------------------------------------------------------------------------
void
IvVertexBufferDX11::Destroy()
{
	if ( mBufferPtr )
	{
		// clear the handle and any associated memory
		mBufferPtr->Release();
		mBufferPtr = 0;
	}

	if (sInputLayout[mVertexFormat])
    {
		if (sInputLayout[mVertexFormat]->Release() == 0)
        {
			sInputLayout[mVertexFormat] = 0;
        }
    }

    mNumVertices = 0;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferDX11::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active vertex buffer
//-------------------------------------------------------------------------------
bool
IvVertexBufferDX11::MakeActive(ID3D11DeviceContext* context)
{
    if ( mBufferPtr == 0 || mNumVertices == 0 )
        return false;
    
	UINT stride = kIvVFSize[mVertexFormat];
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mBufferPtr, &stride, &offset);

	context->IASetInputLayout(sInputLayout[mVertexFormat]);
    
    return true;
}

