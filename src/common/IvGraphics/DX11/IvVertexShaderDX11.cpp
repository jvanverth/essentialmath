//===============================================================================
// @ IvVertexShaderDX11.cpp
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

#include "IvVertexShaderDX11.h"
#include "IvConstantTableDX11.h"

#include "IvDebugger.h"
#include <d3dcompiler.h>
#include <string>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static char const* sDefaultVertexShader[kVertexFormatCount] = {0};

static const char sShaderCPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 color : COLOR0, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.color = color;\n"
"    return Out;\n"
"}\n";

static const char sShaderNPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float4x4 IvNormalMatrix;\n"
"float4   IvDiffuseColor;\n"
"float4   IvLightAmbient;\n"
"float4   IvLightDiffuse;\n"
"float4   IvLightDirection;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 normal : NORMAL, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    float4 transNormal = normalize(mul(IvNormalMatrix, normal));\n"
"    float4 lightValue = IvLightAmbient + IvLightDiffuse*saturate(dot(IvLightDirection,transNormal));\n"
"    Out.color = IvDiffuseColor*lightValue;\n"
"    return Out;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"float4x4 IvModelViewProjectionMatrix;\n"
"float4x4 IvNormalMatrix;\n"
"float4   IvLightAmbient;\n"
"float4   IvLightDiffuse;\n"
"float4   IvLightDirection;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 color : COLOR0, float4 normal : NORMAL, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    float4 transNormal = normalize(mul(IvNormalMatrix, normal));\n"
"    float4 lightValue = IvLightAmbient + IvLightDiffuse*saturate(dot(IvLightDirection,transNormal));\n"
"    Out.color = color*lightValue;\n"
"    return Out;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"float4x4 IvModelViewProjectionMatrix;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"	 float2 uv : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main( float2 uv : TEXCOORD0, float4 color : COLOR0, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.color = color;\n"
"    Out.uv.xy = uv.xy;\n"
"    return Out;\n"
"}\n";

static const char sShaderTNPFormat[] = 
"float4x4 IvModelViewProjectionMatrix;\n"
"float4x4 IvNormalMatrix;\n"
"float4   IvLightAmbient;\n"
"float4   IvLightDiffuse;\n"
"float4   IvLightDirection;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"	 float2 uv : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main( float2 uv : TEXCOORD0, float4 color : COLOR0, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    float4 transNormal = normalize(mul(IvNormalMatrix, normal));\n"
"    float ndotVP = saturate(dot3(IvLightDirection,transNormal));\n"
"    Out.color = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    Out.uv.xy = uv.xy;\n"
"    return Out;\n"
"}\n";

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVertexShaderDX11::IvVertexShaderDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvVertexShaderDX11::IvVertexShaderDX11() : IvVertexShader()
    , mShaderPtr(0)
    , mConstantTable(0)
{
    sDefaultVertexShader[kCPFormat] = sShaderCPFormat;
    sDefaultVertexShader[kNPFormat] = sShaderNPFormat;
    sDefaultVertexShader[kCNPFormat] = sShaderCNPFormat;
    sDefaultVertexShader[kTCPFormat] = sShaderTCPFormat;
    sDefaultVertexShader[kTNPFormat] = sShaderTNPFormat;
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderDX11::~IvVertexShaderDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvVertexShaderDX11::~IvVertexShaderDX11()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderDX11::CreateFromFile()
//-------------------------------------------------------------------------------
// Create a shader from a file
//-------------------------------------------------------------------------------
bool
IvVertexShaderDX11::CreateFromFile(const char* filename, ID3D11Device* device)
{
	// Add the expected extension:
	std::string fullFilename = filename;

	fullFilename = fullFilename + std::string(".hlslv");

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

	if (FAILED(D3DCompileFromFile(wfilename, NULL, NULL, "vs_main", "vs_4_0",
		flags, 0, &code, &errorMessages)))
	{
		if (errorMessages)
		{
			const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());

			DEBUG_OUT("Vertex shader error: ");
			DEBUG_OUT(errors << std::endl);
			errorMessages->Release();
		}
		return false;
	}
	if (errorMessages)
	{
		errorMessages->Release();
	}

	HRESULT result = device->CreateVertexShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &mShaderPtr);
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
// @ IvVertexShaderDX11::CreateFromString()
//-------------------------------------------------------------------------------
// Create a shader from a file
//-------------------------------------------------------------------------------
bool
IvVertexShaderDX11::CreateFromString(const char* string, ID3D11Device* device)
{
	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* code;
	ID3DBlob* errorMessages = NULL;

	// compile the shader to assembly
	if ( FAILED(D3DCompile( string, strlen(string)+1, NULL, NULL, NULL, "vs_main", "vs_4_0", 
							   flags, 0, &code, &errorMessages )) )
    {
		const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
        DEBUG_OUT("Vertex shader error: ");
        DEBUG_OUT(errors << std::endl);
	    errorMessages->Release();
		return false;
    }
	if (errorMessages)
	{
		errorMessages->Release();
	}

	HRESULT result = device->CreateVertexShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &mShaderPtr);
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
// @ IvVertexShaderDX11::CreateDefault()
//-------------------------------------------------------------------------------
// Create a default shader
//-------------------------------------------------------------------------------
bool
IvVertexShaderDX11::CreateDefault(IvVertexFormat format, ID3D11Device* device)
{
    return CreateFromString(sDefaultVertexShader[format], device);
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderDX11::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvVertexShaderDX11::Destroy()
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

