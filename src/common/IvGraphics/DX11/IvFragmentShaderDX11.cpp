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

static const char sShaderHeader[] = 
"#define SAMPLER_2D(x) Texture2D x; SamplerState x##Sampler\n"
"#define TEXTURE(x, uv) x.Sample( x##Sampler, uv )\n"
"\n";

static const char sShaderCPFormat[] =
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return input.color;\n"
"}\n";

static const char sShaderNPFormat[] = 
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return input.color;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return input.color;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"	 float2 uv : TEXCOORD0;\n"
"};\n"
"SAMPLER_2D(defaultTexture);\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return mul(input.color, TEXTURE( defaultTexture, input.uv ));\n"
"}\n";

static const char sShaderTNPFormat[] = 
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"	 float2 uv : TEXCOORD0;\n"
"};\n"
"SAMPLER_2D(defaultTexture);\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return mul(input.color, TEXTURE( defaultTexture, input.uv ));\n"
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

    FILE* fp = fopen(fullFilename.c_str(), "r");

    if (!fp)
    {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    unsigned int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int headerLength = sizeof(sShaderHeader);

    char* shaderSrc = new char[headerLength + length];

    // prepend the header
    memcpy(shaderSrc, sShaderHeader, headerLength - 1);

    length = fread(shaderSrc + headerLength - 1, 1, length, fp);
    shaderSrc[headerLength + length - 1] = 0;
    fclose(fp);

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* code;
	ID3DBlob* errorMessages = NULL;

    size_t foo = sizeof(shaderSrc);
    if (FAILED(D3DCompile(shaderSrc, headerLength + length, NULL, NULL, NULL, "ps_main", "ps_4_0",
        flags, 0, &code, &errorMessages)))
    {
		if (errorMessages)
		{
			const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());

			DEBUG_OUT("Pixel shader error: ");
			DEBUG_OUT(errors << std::endl);
            DEBUG_OUT(shaderSrc << std::endl);
			errorMessages->Release();
		}
        delete [] shaderSrc;
		return false;
	}
	if (errorMessages)
	{
		errorMessages->Release();
	}
    delete [] shaderSrc;

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

    int length = strlen(string);
    int headerLength = sizeof(sShaderHeader);
    char* shaderSrc = new char[headerLength + length];

    // prepend the header
    memcpy(shaderSrc, sShaderHeader, headerLength - 1);
    // now add the string
    memcpy(shaderSrc + headerLength - 1, string, length+1);

	// compile the shader to assembly
    if (FAILED(D3DCompile(shaderSrc, headerLength + length, NULL, NULL, NULL, "ps_main", "ps_4_0",
		flags, 0, &code, &errorMessages)))
	{
        if (errorMessages)
        {
            const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
            DEBUG_OUT("Pixel shader error: ");
            DEBUG_OUT(errors << std::endl);
            DEBUG_OUT(shaderSrc << std::endl);
            errorMessages->Release();
        }
        delete [] shaderSrc;
		return false;
	}
	if (errorMessages)
	{
		errorMessages->Release();
	}
    delete [] shaderSrc;

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
