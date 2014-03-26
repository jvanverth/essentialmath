//===============================================================================
// @ IvVertexShaderD3D9.cpp
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

#include "IvVertexShaderD3D9.h"
#include "IvDebugger.h"
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
"    float4 pos : POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 color : COLOR0, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(pos, IvModelViewProjectionMatrix);\n"
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
"    float4 pos : POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 normal : NORMAL, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(pos, IvModelViewProjectionMatrix);\n"
"    float4 transNormal = normalize(mul(normal, IvNormalMatrix));\n"
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
"    float4 pos : POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 color : COLOR0, float4 normal : NORMAL, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(pos, IvModelViewProjectionMatrix);\n"
"    float4 transNormal = normalize(mul(normal, IvNormalMatrix));\n"
"    float4 lightValue = IvLightAmbient + IvLightDiffuse*saturate(dot(IvLightDirection,transNormal));\n"
"    Out.color = color*lightValue;\n"
"    return Out;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"float4x4 IvModelViewProjectionMatrix;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : POSITION;\n"
"    float4 color : COLOR0;\n"
"	 float2 uv : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main( float2 uv : TEXCOORD0, float4 color : COLOR0, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(pos, IvModelViewProjectionMatrix);\n"
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
"    float4 pos : POSITION;\n"
"    float4 color : COLOR0;\n"
"	 float2 uv : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main( float2 uv : TEXCOORD0, float4 color : COLOR0, float4 pos : POSITION )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(pos, IvModelViewProjectionMatrix);\n"
"    float4 transNormal = normalize(mul(normal, IvNormalMatrix));\n"
"    float ndotVP = saturate(dot3(IvLightDirection,transNormal));\n"
"    Out.color = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    Out.uv.xy = uv.xy;\n"
"    return Out;\n"
"}\n";

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVertexShaderD3D9::IvVertexShaderD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvVertexShaderD3D9::IvVertexShaderD3D9() : IvVertexShader()
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
// @ IvVertexShaderD3D9::~IvVertexShaderD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvVertexShaderD3D9::~IvVertexShaderD3D9()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderD3D9::CreateFromFile()
//-------------------------------------------------------------------------------
// Create a shader from a file
//-------------------------------------------------------------------------------
bool
IvVertexShaderD3D9::CreateFromFile( const char* filename, IDirect3DDevice9* device  )
{
    // Add the expected extension:
    std::string fullFilename = filename;

    fullFilename = fullFilename + std::string(".hlslv");

    FILE* fp = fopen(fullFilename.c_str(), "r");

    if (!fp)
        return false;

    fseek(fp, 0, SEEK_END);
    unsigned int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* shaderSrc = new char[length+1];

    length = fread(shaderSrc, 1, length, fp);
    shaderSrc[length] = 0;
	fclose(fp);

	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	LPD3DXBUFFER code;
	LPD3DXBUFFER errorMessages;

	// compile the shader to assembly
	if ( FAILED(D3DXCompileShader( shaderSrc, length+1, 0, 0, "vs_main", "vs_2_0", 
							   flags, &code, &errorMessages, &mConstantTable )) )
    {
        char* errors = (char*) errorMessages->GetBufferPointer();
        DEBUG_OUT("Vertex shader error: ");
        DEBUG_OUT(errors << std::endl);
	    errorMessages->Release();
		return false;
    }

	HRESULT result = device->CreateVertexShader( (DWORD*)code->GetBufferPointer(), &mShaderPtr );
	code->Release();

	if ( FAILED(result) )
		return false;	
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderD3D9::CreateFromString()
//-------------------------------------------------------------------------------
// Create a shader from a file
//-------------------------------------------------------------------------------
bool
IvVertexShaderD3D9::CreateFromString( const char* string, IDirect3DDevice9* device )
{
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	LPD3DXBUFFER code;
	LPD3DXBUFFER errorMessages;

	// compile the shader to assembly
	if ( FAILED(D3DXCompileShader( string, strlen(string)+1, 0, 0, "vs_main", "vs_2_0", 
							   flags, &code, &errorMessages, &mConstantTable )) )
    {
        char* errors = (char*) errorMessages->GetBufferPointer();
        DEBUG_OUT("Vertex shader error: ");
        DEBUG_OUT(errors << std::endl);
	    errorMessages->Release();
		return false;
    }

	HRESULT result = device->CreateVertexShader( (DWORD*)code->GetBufferPointer(), &mShaderPtr );
	code->Release();

	if ( FAILED(result) )
		return false;	
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderOGL::CreateDefault()
//-------------------------------------------------------------------------------
// Create a default shader
//-------------------------------------------------------------------------------
bool
IvVertexShaderD3D9::CreateDefault( IvVertexFormat format, IDirect3DDevice9* device  )
{
    return CreateFromString(sDefaultVertexShader[format], device);
}

//-------------------------------------------------------------------------------
// @ IvVertexShaderD3D9::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvVertexShaderD3D9::Destroy()
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

