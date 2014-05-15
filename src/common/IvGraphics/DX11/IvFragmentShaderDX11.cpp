//===============================================================================
// @ IvFragmentShaderDX11.cpp
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

#include "IvFragmentShaderDX11.h"
#include "IvConstantTableDX11.h"

#include "IvDebugger.h"
#include <d3dcompiler.h>
#include <string>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static char const* sDefaultFragmentShader[kVertexFormatCount] = {0};

static const char sShaderCPFormat[] =
"float4 ps_main( float4 color : COLOR0 ) : SV_Target\n"
"{\n"
"    return color;\n"
"}\n";

static const char sShaderNPFormat[] = 
"float4 ps_main( float4 color : COLOR0 ) : SV_Target\n"
"{\n"
"    return color;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"float4 ps_main( float4 color : COLOR0 ) : SV_Target\n"
"{\n"
"    return color;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"sampler defaultTexture;\n"
"float4 ps_main( float4 color : COLOR0, float2 uv : TEXCOORD0 ) : SV_Target\n"
"{\n"
"    return mul(color, tex2D( defaultTexture, uv ));\n"
"}\n";

static const char sShaderTNPFormat[] = 
"sampler defaultTexture;\n"
"float4 ps_main( float4 color : COLOR0, float2 uv : TEXCOORD0 ) : SV_Target\n"
"{\n"
"    return mul(color, tex2D( defaultTexture, uv ));\n"
"}\n";

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvFragmentShaderDX11::IvFragmentShaderDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvFragmentShaderDX11::IvFragmentShaderDX11() : IvFragmentShader()
    , mShaderPtr(0)
    , mConstantTable(0)
{
    sDefaultFragmentShader[kCPFormat] = sShaderCPFormat;
    sDefaultFragmentShader[kNPFormat] = sShaderNPFormat;
    sDefaultFragmentShader[kCNPFormat] = sShaderCNPFormat;
    sDefaultFragmentShader[kTCPFormat] = sShaderTCPFormat;
    sDefaultFragmentShader[kTNPFormat] = sShaderTNPFormat;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderDX11::~IvFragmentShaderDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvFragmentShaderDX11::~IvFragmentShaderDX11()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderDX11::CreateFromFile()
//-------------------------------------------------------------------------------
// Load shader from a textfile
//-------------------------------------------------------------------------------
bool
IvFragmentShaderDX11::CreateFromFile(const char* filename, ID3D11Device* device)
{
    // Add the expected extension:
    std::string fullFilename = filename;

    fullFilename = fullFilename + std::string(".hlslf");

	// convert to LPCWSTR
	int len = strlen(filename);
	WCHAR* wfilename = new WCHAR(len + 1);  //*** use scratch allocator for this
	MultiByteToWideChar(0, 0, filename, len, wfilename, len + 1);

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* code;
	ID3DBlob* errorMessages = NULL;

	if (FAILED(D3DCompileFromFile(wfilename, NULL, NULL, "ps_main", "ps_4_0",
		flags, 0, &code, &errorMessages)))
	{
		if (errorMessages)
		{
			const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());

			DEBUG_OUT("Pixel shader error: ");
			DEBUG_OUT(errors << std::endl);
			errorMessages->Release();
		}
		return false;
	}
	if (errorMessages)
	{
		errorMessages->Release();
	}

	HRESULT result = device->CreatePixelShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &mShaderPtr);
	if (SUCCEEDED(result))
	{
		mConstantTable = IvConstantTableDX11::Create(device, code);
	}
	if (FAILED(result) || NULL == mConstantTable)
	{
		code->Release();
		return false;
	}

	code->Release();
	return true;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderDX11::CreateFromString()
//-------------------------------------------------------------------------------
// Create a shader from a string
//-------------------------------------------------------------------------------
bool
IvFragmentShaderDX11::CreateFromString(const char* string, ID3D11Device* device)
{
	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* code;
	ID3DBlob* errorMessages = NULL;

	// compile the shader to assembly
	if (FAILED(D3DCompile(string, strlen(string) + 1, NULL, NULL, NULL, "ps_main", "ps_4_0",
		flags, 0, &code, &errorMessages)))
	{
		const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
		DEBUG_OUT("Pixel shader error: ");
		DEBUG_OUT(errors << std::endl);
		errorMessages->Release();
		return false;
	}
	if (errorMessages)
	{
		errorMessages->Release();
	}

	HRESULT result = device->CreatePixelShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &mShaderPtr);
	if (SUCCEEDED(result))
	{
		mConstantTable = IvConstantTableDX11::Create(device, code);
	}
	if (FAILED(result) || NULL == mConstantTable)
	{
		code->Release();
		return false;
	}

	code->Release();
	return true;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderDX11::CreateDefault()
//-------------------------------------------------------------------------------
// Create default shader
//-------------------------------------------------------------------------------
bool 
IvFragmentShaderDX11::CreateDefault(IvVertexFormat format, ID3D11Device* device)
{
    return CreateFromString(sDefaultFragmentShader[format], device);
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderDX11::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvFragmentShaderDX11::Destroy()
{
	if ( mShaderPtr )
	{
		mShaderPtr->Release();
		mShaderPtr = 0;
	}

    if ( mConstantTable )
    {
		mConstantTable->Release();
		mConstantTable = 0;
    }
}
