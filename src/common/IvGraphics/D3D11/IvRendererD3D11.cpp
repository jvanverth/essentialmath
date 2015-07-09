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

static D3D_PRIMITIVE_TOPOLOGY sPrimTypeMap[kPrimTypeCount];

static IvShaderProgramD3D11* sDefaultShaders[kVertexFormatCount];

static Int32 sBlendSrcFunc[kBlendFuncCount];
static Int32 sBlendDestFunc[kBlendFuncCount];

static Int32 sDepthFunc[kDepthTestCount];

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
	                           ID3D11DepthStencilView* depthStencilTarget) :
	IvRenderer()
{
	mDevice = device;
	mDevice->AddRef();

	mContext = context;
	mContext->AddRef();

	mRenderTarget = renderTarget;
	mRenderTarget->AddRef();

	mDepthStencilTarget = depthStencilTarget;
	mDepthStencilTarget->AddRef();

    mShader = NULL;

	mClearColor[0] = 1.0f;
	mClearColor[1] = 1.0f;
	mClearColor[2] = 1.0f;
	mClearColor[3] = 1.0f;
	mClearDepth = 1.0f;

	mAPI = kD3D11;

	sPrimTypeMap[kPointListPrim] = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	sPrimTypeMap[kLineListPrim] = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	sPrimTypeMap[kLineStripPrim] = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	sPrimTypeMap[kTriangleListPrim] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	sPrimTypeMap[kTriangleStripPrim] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	sBlendSrcFunc[kNoBlendFunc] = D3D11_BLEND_ONE;
	sBlendDestFunc[kNoBlendFunc] = D3D11_BLEND_ZERO;
	sBlendSrcFunc[kOpacityBlendFunc] = D3D11_BLEND_SRC_ALPHA;
	sBlendDestFunc[kOpacityBlendFunc] = D3D11_BLEND_INV_SRC_ALPHA;
	sBlendSrcFunc[kAddBlendFunc] = D3D11_BLEND_ONE;
	sBlendDestFunc[kAddBlendFunc] = D3D11_BLEND_ONE;
	sBlendSrcFunc[kMultiplyBlendFunc] = D3D11_BLEND_ZERO;
	sBlendDestFunc[kMultiplyBlendFunc] = D3D11_BLEND_SRC_COLOR;

	sDepthFunc[kDisableDepthTest] = D3D11_COMPARISON_ALWAYS;
	sDepthFunc[kGreaterDepthTest] = D3D11_COMPARISON_GREATER;
	sDepthFunc[kGreaterEqualDepthTest] = D3D11_COMPARISON_GREATER_EQUAL;
	sDepthFunc[kLessDepthTest] = D3D11_COMPARISON_LESS;
	sDepthFunc[kLessEqualDepthTest] = D3D11_COMPARISON_LESS_EQUAL;

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

	mDepthStencilTarget->Release();
	mDepthStencilTarget = 0;

	mRenderTarget->Release();
	mRenderTarget = 0;

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
// @ IvRendererD3D11::InitGL()
//-------------------------------------------------------------------------------
// Set up base GL parameters
//-------------------------------------------------------------------------------
bool 
IvRendererD3D11::InitD3D11()                                   
{
	/*
    // turn on smooth shading
    mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );    
    */
    // set clear color and depth
    SetClearColor(0.0f, 0.0f, 0.2f, 1.0f);          
    SetClearDepth(1.0f); 
    
    // set up depth buffer
    SetDepthTest(kLessEqualDepthTest);
    SetDepthWrite(true);

	//-------------- For now -------------------------------------

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState* rasterState;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	// Create the rasterizer state from the description we just filled out.
	HRESULT result = mDevice->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		return false;
	}
	// Now set the rasterizer state.
	mContext->RSSetState(rasterState);
	rasterState->Release();

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ID3D11DepthStencilState* depthState;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	// Create the depth stencil state.
	result = mDevice->CreateDepthStencilState(&depthStencilDesc, &depthState);
	if (FAILED(result))
	{
		return false;
	}
	// Set the depth stencil state.
	mContext->OMSetDepthStencilState(depthState, 1);
	depthState->Release();

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
IvRendererD3D11::SetBlendFunc(IvBlendFunc blend)
{
	/*
    if (blend == kNoBlendFunc)
        mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    else
        mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	mDevice->SetRenderState( D3DRS_SRCBLEND, sBlendSrcFunc[blend] );
	mDevice->SetRenderState( D3DRS_DESTBLEND, sBlendDestFunc[blend] );
	*/
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetColorMask()
//-------------------------------------------------------------------------------
// Set which colors will actually be written to the color buffer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetColorMask( bool red, bool green, bool blue, bool alpha )
{
	/*
	unsigned int mask = 0;
	if ( red )
		mask |= D3DCOLORWRITEENABLE_RED;
	if ( green )
		mask |= D3DCOLORWRITEENABLE_GREEN;
	if ( blue )
		mask |= D3DCOLORWRITEENABLE_BLUE;
	if ( alpha )
		mask |= D3DCOLORWRITEENABLE_ALPHA;
	
    mDevice->SetRenderState( D3DRS_COLORWRITEENABLE, mask );
	*/
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetFillMode()
//-------------------------------------------------------------------------------
// Set whether we're in solid or wireframe drawing mode
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetFillMode( IvFillMode fill )
{
	/*
    if (fill == kWireframeFill)
    {
        mDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	    mDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    }
    else if (fill == kSolidFill)
    {
        mDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	    mDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    }
	*/
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
	/*
    if (func == kDisableDepthTest)
        mDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    else
        mDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	mDevice->SetRenderState( D3DRS_ZFUNC, sDepthFunc[func] );
	*/
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::GetDepthTest()
//-------------------------------------------------------------------------------
// Get the depth-testing function
//-------------------------------------------------------------------------------
IvDepthTestFunc IvRendererD3D11::GetDepthTest()
{
	/*
	DWORD mode;
	mDevice->GetRenderState( D3DRS_ZENABLE, &mode );
    if (mode == D3DZB_FALSE)
	{
        return kDisableDepthTest;
	}

	mDevice->GetRenderState( D3DRS_ZFUNC, &mode );
    for (unsigned int func = 0; func < kDepthTestCount; ++func)
    {
        if ( mode == sDepthFunc[func] )
            return (IvDepthTestFunc)(func);
    }
	*/
    ASSERT(false);
    return kLessEqualDepthTest;
	
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetDepthWrite()
//-------------------------------------------------------------------------------
// Enable/Disable the writing of depths to the depth buffer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetDepthWrite(bool write)
{
//	mDevice->SetRenderState( D3DRS_ZWRITEENABLE, write ? TRUE : FALSE );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetWorldMatrix()
//-------------------------------------------------------------------------------
// Sets the world matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetWorldMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetWorldMatrix(matrix);

	//mDevice->SetTransform( D3DTS_WORLDMATRIX(0), (const D3DMATRIX *)&mWorldMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetViewMatrix()
//-------------------------------------------------------------------------------
// Sets the camera matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetViewMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetViewMatrix(matrix);

	//mDevice->SetTransform( D3DTS_VIEW, (const D3DMATRIX *)&mViewMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D11::SetProjectionMatrix()
//-------------------------------------------------------------------------------
// Sets the projection matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D11::SetProjectionMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetProjectionMatrix(matrix);

	//mDevice->SetTransform( D3DTS_PROJECTION, (const D3DMATRIX *)&mProjectionMat );
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
        mResourceManager->Destroy( vs );
        mResourceManager->Destroy( fs );
		if (format == kNPFormat)
		{
			IvUniform* diffuseColor = sDefaultShaders[format]->GetUniform("IvDiffuseColor");
			// defaults to white
			diffuseColor->SetValue(IvVector4(1.0f, 1.0f, 1.0f, 1.0f), 0);
		}
    }

    SetShaderProgram(sDefaultShaders[format]);
}
        