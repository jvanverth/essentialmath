//===============================================================================
// @ IvRendererD3D11.cpp
// 
// Windowing and display setup routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include "IvDebugger.h"
#include "IvAssert.h"
#include "IvIndexBufferD3D11.h"
#include <IvMath.h>
#include "IvMatrix44.h"
#include "IvRendererD3D11.h"
#include "IvResourceManagerD3D11.h"
#include "IvShaderProgramD3D11.h"
#include "IvVertexBufferD3D11.h"
#include "IvUniform.h"

#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static D3D_PRIMITIVE_TOPOLOGY sPrimTypeMap[kPrimTypeCount] =
{
	D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

static IvShaderProgramD3D11* sDefaultShaders[kVertexFormatCount];

static D3D11_BLEND sBlendFunc[kBlendFuncCount] =
{
	D3D11_BLEND_ZERO,
	D3D11_BLEND_ONE,
	D3D11_BLEND_SRC_COLOR,
	D3D11_BLEND_INV_SRC_COLOR,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_DEST_COLOR,
	D3D11_BLEND_INV_DEST_COLOR,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_INV_DEST_ALPHA,
};

static D3D11_BLEND sBlendAlphaFunc[kBlendFuncCount] =
{
	D3D11_BLEND_ZERO,
	D3D11_BLEND_ONE,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_INV_DEST_ALPHA,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_INV_DEST_ALPHA,
};

static D3D11_BLEND_OP sBlendOp[kBlendOpCount] =
{
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_OP_SUBTRACT,
	D3D11_BLEND_OP_MIN,
	D3D11_BLEND_OP_MAX,
};

static D3D11_COMPARISON_FUNC sDepthFunc[kDepthTestCount] =
{
	D3D11_COMPARISON_ALWAYS,
	D3D11_COMPARISON_GREATER,
	D3D11_COMPARISON_GREATER_EQUAL,
	D3D11_COMPARISON_LESS,
	D3D11_COMPARISON_LESS_EQUAL
};

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvRenderer::Create()
//-------------------------------------------------------------------------------
// Static constructor
//-------------------------------------------------------------------------------
bool 
IvRendererD3D11::Create(ID3D11Device* device, ID3D11DeviceContext* context, 
                        ID3D11RenderTargetView* renderTarget, 
                        ID3D11DepthStencilView* depthStencilTarget)
{
    if (!device || !context || !renderTarget || !depthStencilTarget)
    {
        return false;
    }
    
	if (!mRenderer)
	{
        mRenderer = new IvRendererD3D11(device, context, renderTarget, depthStencilTarget);
	}
    return ( mRenderer != 0 );

}   // End of IvRenderer::Create()


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::IvRendererD3D11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvRendererD3D11::IvRendererD3D11(ID3D11Device* device, ID3D11DeviceContext* context,
                                 ID3D11RenderTargetView* renderTarget, 
                                 ID3D11DepthStencilView* depthStencilTarget)
    : IvRenderer()
    , mRenderTarget(NULL)
    , mDepthStencilTarget(NULL)
{
	mDevice = device;
	mDevice->AddRef();

	mContext = context;
	mContext->AddRef();

    SetTargets(renderTarget, depthStencilTarget);

    mShader = NULL;

	mClearColor[0] = 1.0f;
	mClearColor[1] = 1.0f;
	mClearColor[2] = 1.0f;
	mClearColor[3] = 1.0f;
	mClearDepth = 1.0f;

	mSrcBlend = mSrcBlendAlpha = D3D11_BLEND_ONE;
	mDestBlend = mDestBlendAlpha = D3D11_BLEND_ZERO;
	mBlendOp = D3D11_BLEND_OP_ADD;
	mWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	mAPI = kD3D11;

}   // End of IvRendererD3D11::IvRendererD3D11()


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::~IvRendererD3D11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvRendererD3D11::~IvRendererD3D11()
{
	// delete shader program?
    for ( unsigned int i = 0; i < kVertexFormatCount; ++i )
    {
        if (sDefaultShaders[i])
        {
            mResourceManager->Destroy(sDefaultShaders[i]);
            sDefaultShaders[i] = 0;
        }
    }

    if (mResourceManager)
    {
        delete (IvResourceManagerD3D11*)(mResourceManager);
        mResourceManager = 0;
    }

    ReleaseTargets();

	mContext->Release();
	mContext = 0;

	mDevice->Release();
	mDevice = 0;

}   // End of IvRendererD3D11::~IvRendererD3D11()


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::Initialize()
//-------------------------------------------------------------------------------
// Initialize display
//-------------------------------------------------------------------------------
bool
IvRendererD3D11::Initialize( unsigned int width, unsigned int height )
{
    if (!InitD3D11())
        return false;

    // set parameters for window size
    Resize( width, height );
    
    // create resource manager
    mResourceManager = new IvResourceManagerD3D11( mDevice );

    return true;
    
}   // End of IvRendererD3D11::Initialize()
    

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::Resize()
//-------------------------------------------------------------------------------
// Set up parameters for window size
//-------------------------------------------------------------------------------
void 
IvRendererD3D11::Resize(unsigned int width, unsigned int height ) 
{
	// prevent divide by zero
    if (height == 0)                                    
    {
        height = 1;                                 
    }

    // set up current viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mContext->RSSetViewports(1, &vp);
    mWidth = width;
    mHeight = height;

    // set default projection matrix
    float d = 1.0f/IvTan(mFOV/180.0f*kPI*0.5f);
    float Q = mFar/(mFar-mNear);
    IvMatrix44 perspective;

    perspective(0,0) = d/((float)width/(float)height);
    perspective(1,1) = d;
    perspective(2,2) = Q;
    perspective(2,3) = -mNear*Q;
    perspective(3,2) = 1.0f;
    perspective(3,3) = 0.0f;

    SetProjectionMatrix(perspective);

    IvMatrix44 ident;
    SetViewMatrix(ident);
    SetWorldMatrix(ident);

}   // End of IvRendererD3D11::Resize()


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetTargets()
//-------------------------------------------------------------------------------
// Set and add refs to render and depth stencil targets
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetTargets(ID3D11RenderTargetView* renderTarget,
    ID3D11DepthStencilView* depthStencilTarget)
{
    mRenderTarget = renderTarget;
    mRenderTarget->AddRef();

    mDepthStencilTarget = depthStencilTarget;
    mDepthStencilTarget->AddRef();
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::ReleaseTargets()
//-------------------------------------------------------------------------------
// Release refs to render and depth stencil targets
//-------------------------------------------------------------------------------
void IvRendererD3D11::ReleaseTargets()
{
    if (mDepthStencilTarget)
    {
        mDepthStencilTarget->Release();
        mDepthStencilTarget = 0;
    }

    if (mRenderTarget)
    {
        mRenderTarget->Release();
        mRenderTarget = 0;
    }
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::InitD3D11()
//-------------------------------------------------------------------------------
// Set up base D3D parameters
//-------------------------------------------------------------------------------
bool 
IvRendererD3D11::InitD3D11()                                   
{
	/*
    // turn on smooth shading
    mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );    
    */
    // set clear color and depth
    SetClearColor(0.0f, 0.0f, 0.1f, 1.0f);          
    SetClearDepth(1.0f); 
    
    // set up depth buffer
    SetDepthTest(kLessEqualDepthTest);
    SetDepthWrite(true);

	// Set solid fill
	SetFillMode(kSolidFill);

	// Initialize the depth-stencil state.
	mDepthTestFunc = kLessEqualDepthTest;
	mDepthWrite = TRUE;
	UpdateDepthStencilState();

	// Initialize the blend state
	mSrcBlend = D3D11_BLEND_ONE;
	mDestBlend = D3D11_BLEND_ZERO;
	mBlendOp = D3D11_BLEND_OP_ADD;
	mWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	UpdateBlendState();

	/*
    // set point size
    float pointSize = 5.0f;
    mDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&pointSize) );
	*/
    return true;                                        

}   // End of IvRendererD3D11::InitGL()


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetClearColor()
//-------------------------------------------------------------------------------
// Set background color
//-------------------------------------------------------------------------------
void 
IvRendererD3D11::SetClearColor( float red, float green, float blue, float alpha )  
{
    // set clear color
	mClearColor[0] = red;
	mClearColor[1] = green;
	mClearColor[2] = blue;
	mClearColor[3] = alpha;
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetClearDepth()
//-------------------------------------------------------------------------------
// Set base depth value
//-------------------------------------------------------------------------------
void 
IvRendererD3D11::SetClearDepth( float depth )  
{
    // set clear depth
    mClearDepth = depth; 
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::ClearBuffers()
//-------------------------------------------------------------------------------
// Clear necessary buffers
//-------------------------------------------------------------------------------
void 
IvRendererD3D11::ClearBuffers(IvClearBuffer buffer)
{
    if (!mRenderTarget || !mDepthStencilTarget)
    {
        return;
    }

    switch (buffer)
    {
        case kColorClear:
			mContext->ClearRenderTargetView(mRenderTarget, mClearColor);
            break;
        case kDepthClear:
			mContext->ClearDepthStencilView(mDepthStencilTarget, D3D11_CLEAR_DEPTH, mClearDepth, 0);
            break;
        case kColorDepthClear:
			mContext->ClearRenderTargetView(mRenderTarget, mClearColor);
			mContext->ClearDepthStencilView(mDepthStencilTarget, D3D11_CLEAR_DEPTH, mClearDepth, 0);
			break;
    };
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetBlendFunc()
//-------------------------------------------------------------------------------
// Set the pixel-blending function
//-------------------------------------------------------------------------------
void 
IvRendererD3D11::SetBlendFunc(IvBlendFunc srcBlend, IvBlendFunc destBlend, IvBlendOp op)
{
	mSrcBlend = sBlendFunc[srcBlend];
	mDestBlend = sBlendFunc[destBlend];
	mSrcBlendAlpha = sBlendAlphaFunc[srcBlend];
	mDestBlendAlpha = sBlendAlphaFunc[destBlend];
	mBlendOp = sBlendOp[op];

	UpdateBlendState();
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetColorMask()
//-------------------------------------------------------------------------------
// Set which colors will actually be written to the color buffer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetColorMask( bool red, bool green, bool blue, bool alpha )
{
	unsigned char mask = 0;
	if (red)
		mask |= D3D11_COLOR_WRITE_ENABLE_RED;
	if (green)
		mask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	if (blue)
		mask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	if (alpha)
		mask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	mWriteMask = mask;

	UpdateBlendState();
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::UpdateBlendState()
//-------------------------------------------------------------------------------
// Update the blend state
//-------------------------------------------------------------------------------
void IvRendererD3D11::UpdateBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	ID3D11BlendState* blendState;

	// ideally we'd only call this at most once per draw
	// the assumption is that the client will only be either setting blend funcs
	// or the write mask
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable =
		!(D3D11_BLEND_ONE == mSrcBlend && D3D11_BLEND_ZERO == mDestBlend && D3D11_BLEND_OP_ADD == mBlendOp);
	blendDesc.RenderTarget[0].SrcBlend = mSrcBlend;
	blendDesc.RenderTarget[0].DestBlend = mDestBlend;
	blendDesc.RenderTarget[0].BlendOp = mBlendOp;
	blendDesc.RenderTarget[0].SrcBlendAlpha = mSrcBlendAlpha;
	blendDesc.RenderTarget[0].DestBlendAlpha = mDestBlendAlpha;
	blendDesc.RenderTarget[0].BlendOpAlpha = mBlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = mWriteMask;

	// Create the blend state from the description we just filled out.
	HRESULT result = mDevice->CreateBlendState(&blendDesc, &blendState);
	if (FAILED(result))
	{
		ASSERT(false);
		return;
	}
	// Now set the blend state.
	mContext->OMSetBlendState(blendState, NULL, 0xffffffff);
	blendState->Release();
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetFillMode()
//-------------------------------------------------------------------------------
// Set whether we're in solid or wireframe drawing mode
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetFillMode( IvFillMode fill )
{
	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState* rasterState;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = (fill == kSolidFill) ? D3D11_CULL_BACK : D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = (fill == kSolidFill) ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	// Create the rasterizer state from the description we just filled out.
	HRESULT result = mDevice->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		return;
	}
	// Now set the rasterizer state.
	mContext->RSSetState(rasterState);
	rasterState->Release();
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetShadeMode(IvShadeMode shade)
{
	/*
    if (shade == kFlatShaded)
        mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
    else if (shade == kGouraudShaded)
        mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	*/
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::GetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
IvShadeMode 
IvRendererD3D11::GetShadeMode()
{
	/*
	DWORD mode;
	mDevice->GetRenderState( D3DRS_SHADEMODE, &mode );
    if (mode == D3DSHADE_FLAT)
	{
        return kFlatShaded;
	}
    else if (mode == D3DSHADE_GOURAUD)
	{
		return kGouraudShaded;
	}
	else*/
	{
//		ASSERT(false);
		return kGouraudShaded;
	}
	
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetDepthTest()
//-------------------------------------------------------------------------------
// Set the depth-testing function
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetDepthTest(IvDepthTestFunc func)
{
	mDepthTestFunc = func;
	mDepthWrite = (func != kDisableDepthTest);
	UpdateDepthStencilState();
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::GetDepthTest()
//-------------------------------------------------------------------------------
// Get the depth-testing function
//-------------------------------------------------------------------------------
IvDepthTestFunc IvRendererD3D11::GetDepthTest()
{
    return mDepthTestFunc;
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetDepthWrite()
//-------------------------------------------------------------------------------
// Enable/Disable the writing of depths to the depth buffer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetDepthWrite(bool write)
{
	mDepthWrite = write;
	UpdateDepthStencilState();
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::UpdateDepthStencilState()
//-------------------------------------------------------------------------------
// Internal function to update depth stencil params
//-------------------------------------------------------------------------------
void IvRendererD3D11::UpdateDepthStencilState()
{
	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ID3D11DepthStencilState* depthState;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = mDepthWrite;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = sDepthFunc[mDepthTestFunc];
	depthStencilDesc.StencilEnable = false;
	// Create the depth stencil state.
	HRESULT result = mDevice->CreateDepthStencilState(&depthStencilDesc, &depthState);
	if (FAILED(result))
	{
		ASSERT(false);
		return;
	}
	// Set the depth stencil state.
	mContext->OMSetDepthStencilState(depthState, 1);
	depthState->Release();
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetWorldMatrix()
//-------------------------------------------------------------------------------
// Sets the world matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetWorldMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetWorldMatrix(matrix);
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetViewMatrix()
//-------------------------------------------------------------------------------
// Sets the camera matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetViewMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetViewMatrix(matrix);
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetProjectionMatrix()
//-------------------------------------------------------------------------------
// Sets the projection matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetProjectionMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetProjectionMatrix(matrix);
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::GetShaderProgram()
//-------------------------------------------------------------------------------
// Returns the currently-active shader program
//-------------------------------------------------------------------------------
IvShaderProgram* IvRendererD3D11::GetShaderProgram()
{
    return mShader;
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetShaderProgram()
//-------------------------------------------------------------------------------
// Applies a shader program
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetShaderProgram(IvShaderProgram* program)
{
    // This is a safe cast, since we will only link IvShaderProgramD3D11 with the
    // D3D11 renderer.
    mShader = static_cast<IvShaderProgramD3D11*>(program);
    if (mShader)
    {
        mShader->MakeActive( mContext );
    }
    else
    {
        // Otherwise (NULL shader), we will need to bind a default shader later 
		mContext->VSSetShader(NULL, NULL, 0);
		mContext->VSSetShader(NULL, NULL, 0);
    }
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererD3D11::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
                  IvIndexBuffer* indexBuffer, unsigned int numIndices)
{
    BindDefaultShaderIfNeeded(vertexBuffer->GetVertexFormat());

	// update any default uniforms
	if ( mShader )
	{
		IvUniform* modelviewproj = mShader->GetUniform("IvModelViewProjectionMatrix");
		if ( modelviewproj )
		{
			modelviewproj->SetValue( mWVPMat, 0 );
		}
		IvUniform* normalMat = mShader->GetUniform("IvNormalMatrix");
		if ( normalMat )
		{
			normalMat->SetValue( mNormalMat, 0 );
		}
		IvUniform* diffuseColor = mShader->GetUniform("IvDiffuseColor");
		if (diffuseColor)
		{
			diffuseColor->SetValue(mDiffuseColor,0);
		}
		IvUniform* ambient = mShader->GetUniform("IvLightAmbient");
		if ( ambient )
		{
			ambient->SetValue(mLightAmbient,0);
		}
		IvUniform* diffuse = mShader->GetUniform("IvLightDiffuse");
		if ( diffuse )
		{
			diffuse->SetValue(mLightDiffuse,0);
		}
		IvUniform* direction = mShader->GetUniform("IvLightDirection");
		if ( direction )
		{
			direction->SetValue(mLightDirection,0);
		}
	}

	mShader->BindUniforms( mContext );

	if (vertexBuffer)
        static_cast<IvVertexBufferD3D11*>(vertexBuffer)->MakeActive( mContext );
    else
        return;

    if (indexBuffer)
		static_cast<IvIndexBufferD3D11*>(indexBuffer)->MakeActive( mContext );
    else
        return;

	mContext->IASetPrimitiveTopology(sPrimTypeMap[primType]);
	mContext->DrawIndexed(numIndices, 0, 0);
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererD3D11::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices)
{
    BindDefaultShaderIfNeeded(vertexBuffer->GetVertexFormat());

	// update any default uniforms
	if ( mShader )
	{
		IvUniform* modelviewproj = mShader->GetUniform("IvModelViewProjectionMatrix");
		if ( modelviewproj )
		{
			modelviewproj->SetValue(mWVPMat, 0);
		}
		IvUniform* normalMat = mShader->GetUniform("IvNormalMatrix");
		if ( normalMat )
		{
			normalMat->SetValue(mNormalMat, 0);
		}
		IvUniform* diffuseColor = mShader->GetUniform("IvDiffuseColor");
		if (diffuseColor)
		{
			diffuseColor->SetValue(mDiffuseColor,0);
		}
		IvUniform* ambient = mShader->GetUniform("IvLightAmbient");
		if ( ambient )
		{
			ambient->SetValue(mLightAmbient,0);
		}
		IvUniform* diffuse = mShader->GetUniform("IvLightDiffuse");
		if ( diffuse )
		{
			diffuse->SetValue(mLightDiffuse,0);
		}
		IvUniform* direction = mShader->GetUniform("IvLightDirection");
		if ( direction )
		{
			direction->SetValue(mLightDirection,0);
		}
	}

	mShader->BindUniforms(mContext);

    if (vertexBuffer)
        static_cast<IvVertexBufferD3D11*>(vertexBuffer)->MakeActive( mContext );
    else
        return;

	mContext->IASetPrimitiveTopology(sPrimTypeMap[primType]);
	mContext->Draw(numVertices, 0);
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::BindDefaultShaderIfNeeded()
//-------------------------------------------------------------------------------
// Selects a default shader for the given vertex format if none is bound
//-------------------------------------------------------------------------------
void IvRendererD3D11::BindDefaultShaderIfNeeded(IvVertexFormat format)
{
    if (mShader)
        return;

    if (!sDefaultShaders[format])
    {
        IvVertexShader* vs = mResourceManager->CreateDefaultVertexShader(format); 
        IvFragmentShader* fs = mResourceManager->CreateDefaultFragmentShader(format);

        sDefaultShaders[format] = static_cast<IvShaderProgramD3D11*>(
            mResourceManager->CreateShaderProgram( vs, fs ));

		if (format == kNPFormat)
		{
			IvUniform* diffuseColor = sDefaultShaders[format]->GetUniform("IvDiffuseColor");
			// defaults to white
			diffuseColor->SetValue(IvVector4(1.0f, 1.0f, 1.0f, 1.0f), 0);
		}
    }

    SetShaderProgram(sDefaultShaders[format]);
}
        