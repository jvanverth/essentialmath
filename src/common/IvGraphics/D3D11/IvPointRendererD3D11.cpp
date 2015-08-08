//===============================================================================
// @ IvPointRendererD3D11.cpp
// 
// D3D11 code for handling sized points
// ------------------------------------------------------------------------------
// Copyright (C) 2015  James M. Van Verth & Lars M. Bishop. 
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <d3d11.h>
#include <d3dcompiler.h>

#include "IvPointRendererD3D11.h"

#include "IvAssert.h"
#include "IvDebugger.h"
#include "IvVertexBufferD3D11.h"
#include "IvRendererD3D11.h"
#include "IvResourceManager.h"
#include "IvShaderProgram.h"
#include "IvUniform.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static ID3D11InputLayout* sInputLayout[kVertexFormatCount] = { 0 };

static D3D11_INPUT_ELEMENT_DESC sPFormatElements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static D3D11_INPUT_ELEMENT_DESC sCPFormatElements[] =
{
    { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static D3D11_INPUT_ELEMENT_DESC sNPFormatElements[] =
{
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static D3D11_INPUT_ELEMENT_DESC sCNPFormatElements[] =
{
    { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static D3D11_INPUT_ELEMENT_DESC sTCPFormatElements[] =
{
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static D3D11_INPUT_ELEMENT_DESC sTNPFormatElements[] =
{
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static char const* sVertexShader[kVertexFormatCount] = { 0 };

static const char sVertexShaderPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float    pointScale;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"};\n"
"VS_OUTPUT vs_main( float4 pos : POSITION0, float4 quadpos : POSITION1 )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.pos.xyz /= Out.pos.w;\n"
"    Out.pos.xy += pointScale*quadpos.xy;\n"
"    Out.pos.w = 1.0;\n"
"    return Out;\n"
"}\n";

static const char sVertexShaderCPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float    pointScale;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 color : COLOR0, float4 pos : POSITION, float4 quadpos : POSITION1 )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.pos.xyz /= Out.pos.w;\n"
"    Out.pos.xy += pointScale*quadpos.xy;\n"
"    Out.pos.w = 1.0;\n"
"    Out.color = color;\n"
"    return Out;\n"
"}\n";

static const char sVertexShaderNPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float4x4 IvNormalMatrix;\n"
"float4   IvDiffuseColor;\n"
"float4   IvLightAmbient;\n"
"float4   IvLightDiffuse;\n"
"float4   IvLightDirection;\n"
"float    pointScale;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 normal : NORMAL, float4 pos : POSITION, float4 quadpos : POSITION1 )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.pos.xyz /= Out.pos.w;\n"
"    Out.pos.xy += pointScale*quadpos.xy;\n"
"    Out.pos.w = 1.0;\n"
"    float4 transNormal = normalize(mul(IvNormalMatrix, normal));\n"
"    float4 lightValue = IvLightAmbient + IvLightDiffuse*saturate(dot(IvLightDirection,transNormal));\n"
"    Out.color = IvDiffuseColor*lightValue;\n"
"    return Out;\n"
"}\n";

static const char sVertexShaderCNPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float4x4 IvNormalMatrix;\n"
"float4   IvLightAmbient;\n"
"float4   IvLightDiffuse;\n"
"float4   IvLightDirection;\n"
"float    pointScale;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"VS_OUTPUT vs_main( float4 color : COLOR0, float4 normal : NORMAL, float4 pos : POSITION, float4 quadpos : POSITION1 )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.pos.xyz /= Out.pos.w;\n"
"    Out.pos.xy += pointScale*quadpos.xy;\n"
"    Out.pos.w = 1.0;\n"
"    float4 transNormal = normalize(mul(IvNormalMatrix, normal));\n"
"    float4 lightValue = IvLightAmbient + IvLightDiffuse*saturate(dot(IvLightDirection,transNormal));\n"
"    Out.color = color*lightValue;\n"
"    return Out;\n"
"}\n";

static const char sVertexShaderTCPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float    pointScale;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"    float2 uv : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main( float2 uv : TEXCOORD0, float4 color : COLOR0, float4 pos : POSITION, float4 quadpos : POSITION1 )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.pos.xyz /= Out.pos.w;\n"
"    Out.pos.xy += pointScale*quadpos.xy;\n"
"    Out.pos.w = 1.0;\n"
"    Out.color = color;\n"
"    Out.uv.xy = uv.xy;\n"
"    return Out;\n"
"}\n";

static const char sVertexShaderTNPFormat[] =
"float4x4 IvModelViewProjectionMatrix;\n"
"float4x4 IvNormalMatrix;\n"
"float4   IvLightAmbient;\n"
"float4   IvLightDiffuse;\n"
"float4   IvLightDirection;\n"
"float    pointScale;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"    float2 uv : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT vs_main( float2 uv : TEXCOORD0, float3 normal : NORMAL, float4 pos : POSITION, float4 quadpos : POSITION1 )\n"
"{\n"
"    VS_OUTPUT Out = (VS_OUTPUT) 0;\n"
"    Out.pos = mul(IvModelViewProjectionMatrix, pos);\n"
"    Out.pos.xyz /= Out.pos.w;\n"
"    Out.pos.xy += pointScale*quadpos.xy;\n"
"    Out.pos.w = 1.0;\n"
"    float4 transNormal = normalize(mul(IvNormalMatrix, normal));\n"
"    float ndotVP = saturate(dot(IvLightDirection,transNormal));\n"
"    Out.color = IvLightAmbient + IvLightDiffuse*ndotVP;\n"
"    Out.uv.xy = uv.xy;\n"
"    return Out;\n"
"}\n";

static char const* sFragmentShader[kVertexFormatCount] = { 0 };

static const char sFragmentShaderP[] =
"float4   IvDiffuseColor;\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"};\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return IvDiffuseColor;\n"
"}\n";

static const char sFragmentShaderCP[] =
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"};\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return input.color;\n"
"}\n";

static const char sFragmentShaderTCP[] =
"struct VS_OUTPUT\n"
"{\n"
"    float4 pos : SV_POSITION;\n"
"    float4 color : COLOR0;\n"
"    float2 uv : TEXCOORD0;\n"
"};\n"
"SAMPLER_2D(defaultTexture);\n"
"float4 ps_main( VS_OUTPUT input ) : SV_Target\n"
"{\n"
"    return mul(input.color, TEXTURE( defaultTexture, input.uv ));\n"
"}\n";

static IvShaderProgram* sShaders[kVertexFormatCount] = { 0 };

static IvVertexBuffer* sQuadBuffer = 0;

static float sPointSize = 2.0f;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

void IvPointRendererD3D11::Setup()
{
    sVertexShader[kPFormat] = sVertexShaderPFormat;
    sVertexShader[kCPFormat] = sVertexShaderCPFormat;
    sVertexShader[kNPFormat] = sVertexShaderNPFormat;
    sVertexShader[kCNPFormat] = sVertexShaderCNPFormat;
    sVertexShader[kTCPFormat] = sVertexShaderTCPFormat;
    sVertexShader[kTNPFormat] = sVertexShaderTNPFormat;

    sFragmentShader[kPFormat] = sFragmentShaderP;
    sFragmentShader[kCPFormat] = sFragmentShaderCP;
    sFragmentShader[kNPFormat] = sFragmentShaderCP;
    sFragmentShader[kCNPFormat] = sFragmentShaderCP;
    sFragmentShader[kTCPFormat] = sFragmentShaderTCP;
    sFragmentShader[kTNPFormat] = sFragmentShaderTCP;

    for (int format = 0; format < kVertexFormatCount; ++format)
    {
        // create the shaders
        sShaders[format] = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
            IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromString(
            sVertexShader[format]),
            IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromString(
            sFragmentShader[format]));

        // create the vertex layout
        D3D11_INPUT_ELEMENT_DESC* elements = NULL;
        UINT numElements = 0;
        const char* shaderString = NULL;

        switch (format)
        {
        case kPFormat:
            elements = sPFormatElements;
            numElements = sizeof(sPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC);
            shaderString = sVertexShaderPFormat;
            break;
        case kCPFormat:
            elements = sCPFormatElements;
            numElements = sizeof(sCPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC);
            shaderString = sVertexShaderCPFormat;
            break;
        case kNPFormat:
            elements = sNPFormatElements;
            numElements = sizeof(sNPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC);
            shaderString = sVertexShaderNPFormat;
            break;
        case kCNPFormat:
            elements = sCNPFormatElements;
            numElements = sizeof(sCNPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC);
            shaderString = sVertexShaderCNPFormat;
            break;
        case kTCPFormat:
            elements = sTCPFormatElements;
            numElements = sizeof(sTCPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC);
            shaderString = sVertexShaderTCPFormat;
            break;
        case kTNPFormat:
            elements = sTNPFormatElements;
            numElements = sizeof(sTNPFormatElements) / sizeof(D3D11_INPUT_ELEMENT_DESC);
            shaderString = sVertexShaderTNPFormat;
            break;
        }

        ID3DBlob* code;
        ID3DBlob* errorMessages = NULL;

        // compile the shader to assembly
        // it's not ideal to compile the vertex shader again, but we only have to
        // go through this process once to create the layout
        if (FAILED(D3DCompile(shaderString, strlen(shaderString) + 1, NULL, NULL, NULL, "vs_main", "vs_4_0",
            D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &code, &errorMessages)))
        {
            const char* errors = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
            DEBUG_OUT("Vertex shader error: ");
            DEBUG_OUT(errors << std::endl);
            errorMessages->Release();
            ASSERT(false);
            return;
        }
        if (errorMessages)
        {
            errorMessages->Release();
        }

        ID3D11Device* device = static_cast<IvRendererD3D11*>(IvRenderer::mRenderer)->GetDevice();
        if (FAILED(device->CreateInputLayout(elements, numElements, code->GetBufferPointer(), code->GetBufferSize(), &sInputLayout[format])))
        {
            code->Release();
            ASSERT(false);
            return;
        }
        code->Release();
    }

    // create the quad buffer
    float xCoord = 1.0f / static_cast<float>(IvRenderer::mRenderer->GetWidth());
    float yCoord = 1.0f / static_cast<float>(IvRenderer::mRenderer->GetHeight());
    IvPVertex verts[4] =
    {
        IvVector3(xCoord, -yCoord, 0.0f),
        IvVector3(xCoord, yCoord, 0.0f),
        IvVector3(-xCoord, -yCoord, 0.0f),
        IvVector3(-xCoord, yCoord, 0.0f)
    };
    sQuadBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kPFormat, 4, verts, kImmutableUsage);
}

void IvPointRendererD3D11::Teardown()
{
    for (int format = 0; format < kVertexFormatCount; ++format)
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(sShaders[format]);
        sInputLayout[format]->Release();
    }
    IvRenderer::mRenderer->GetResourceManager()->Destroy(sQuadBuffer);
}

void IvPointRendererD3D11::SetPointSize(float size)
{
    sPointSize = size;
}

void IvPointRendererD3D11::SetShaderAndVertexBuffer(ID3D11DeviceContext* context,
                                                    IvVertexFormat format, IvVertexBuffer* buffer)
{
    IvRenderer::mRenderer->SetShaderProgram(sShaders[format]);
    sShaders[format]->GetUniform("pointScale")->SetValue(sPointSize, 0);

    ID3D11Buffer* instancePtr = reinterpret_cast<IvVertexBufferD3D11*>(buffer)->GetBufferPtr();
    ID3D11Buffer* quadPtr = reinterpret_cast<IvVertexBufferD3D11*>(sQuadBuffer)->GetBufferPtr();    
    ID3D11Buffer* buffers[2] = { instancePtr, quadPtr };
    UINT strides[2] = { kIvVFSize[format], kIvVFSize[kPFormat] };
    UINT offsets[2] = { 0, 0 };

    context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
    context->IASetInputLayout(sInputLayout[format]);
}

void IvPointRendererD3D11::Draw(ID3D11DeviceContext* context, int numVertices)
{
    // use triangle strip topology
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // draw instanced
    context->DrawInstanced(4, numVertices, 0, 0);
}

