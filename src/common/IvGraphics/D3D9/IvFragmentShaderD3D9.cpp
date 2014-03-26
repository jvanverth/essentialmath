//===============================================================================
// @ IvFragmentShaderD3D9.cpp
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

#include "IvFragmentShaderD3D9.h"
#include "IvDebugger.h"
#include <string>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static char const* sDefaultFragmentShader[kVertexFormatCount] = {0};

static const char sShaderCPFormat[] = 
"float4 ps_main( float4 color : COLOR0 ) : COLOR\n"
"{\n"
"    return color;\n"
"}\n";

static const char sShaderNPFormat[] = 
"float4 ps_main( float4 color : COLOR0 ) : COLOR\n"
"{\n"
"    return color;\n"
"}\n";

static const char sShaderCNPFormat[] = 
"float4 ps_main( float4 color : COLOR0 ) : COLOR\n"
"{\n"
"    return color;\n"
"}\n";

static const char sShaderTCPFormat[] = 
"sampler defaultTexture;\n"
"float4 ps_main( float4 color : COLOR0, float2 uv : TEXCOORD0 ) : COLOR\n"
"{\n"
"    return mul(color, tex2D( defaultTexture, uv ));\n"
"}\n";

static const char sShaderTNPFormat[] = 
"sampler defaultTexture;\n"
"float4 ps_main( float4 color : COLOR0, float2 uv : TEXCOORD0 ) : COLOR\n"
"{\n"
"    return mul(color, tex2D( defaultTexture, uv ));\n"
"}\n";

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvFragmentShaderD3D9::IvFragmentShaderD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvFragmentShaderD3D9::IvFragmentShaderD3D9() : IvFragmentShader()
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
// @ IvFragmentShaderD3D9::~IvFragmentShaderD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvFragmentShaderD3D9::~IvFragmentShaderD3D9()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderD3D9::CreateFromFile()
//-------------------------------------------------------------------------------
// Load shader from a textfile
//-------------------------------------------------------------------------------
bool
IvFragmentShaderD3D9::CreateFromFile( const char* filename, IDirect3DDevice9* device  )
{
    // Add the expected extension:
    std::string fullFilename = filename;

    fullFilename = fullFilename + std::string(".hlslf");

    FILE* fp = fopen(fullFilename.c_str(), "r");

    if (!fp)
        return false;

    fseek(fp, 0, SEEK_END);
    unsigned int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* shaderSrc = new char[length+1];

    length = fread(shaderSrc, 1, length, fp);
    shaderSrc[length] = 0;

	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	LPD3DXBUFFER code;
	LPD3DXBUFFER errorMessages;

	// compile the shader to assembly
	if ( FAILED(D3DXCompileShader( shaderSrc, length+1, 0, 0, "ps_main", "ps_2_0", 
							   flags, &code, &errorMessages, &mConstantTable )) )
    {
        char* errors = (char*) errorMessages->GetBufferPointer();
        DEBUG_OUT("Fragment shader error: ");
        DEBUG_OUT(errors << std::endl);
	    errorMessages->Release();
		return false;
    }

	HRESULT result = device->CreatePixelShader( (DWORD*)code->GetBufferPointer(), &mShaderPtr );
	code->Release();

	if ( FAILED(result) )
		return false;	
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderD3D9::CreateFromString()
//-------------------------------------------------------------------------------
// Create a shader from a string
//-------------------------------------------------------------------------------
bool
IvFragmentShaderD3D9::CreateFromString( const char* string, IDirect3DDevice9* device  )
{
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	LPD3DXBUFFER code;
	LPD3DXBUFFER errorMessages;

	// compile the shader to assembly
	if ( FAILED(D3DXCompileShader( string, strlen(string)+1, 0, 0, "ps_main", "ps_2_0", 
							   flags, &code, &errorMessages, &mConstantTable )) )
    {
        char* errors = (char*) errorMessages->GetBufferPointer();
        DEBUG_OUT("Fragment shader error: ");
        DEBUG_OUT(errors << std::endl);
	    errorMessages->Release();
		return false;
    }

	HRESULT result = device->CreatePixelShader( (DWORD*)code->GetBufferPointer(), &mShaderPtr );
	code->Release();

	if ( FAILED(result) )
		return false;	
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderD3D9::CreateDefault()
//-------------------------------------------------------------------------------
// Create default shader
//-------------------------------------------------------------------------------
bool 
IvFragmentShaderD3D9::CreateDefault( IvVertexFormat format, IDirect3DDevice9* device  )
{
    return CreateFromString(sDefaultFragmentShader[format], device);
}

//-------------------------------------------------------------------------------
// @ IvFragmentShaderD3D9::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvFragmentShaderD3D9::Destroy()
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
