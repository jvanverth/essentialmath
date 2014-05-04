//===============================================================================
// @ IvRendererDX11.cpp
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
#include "IvIndexBufferDX11.h"
#include <IvMath.h>
#include "IvMatrix44.h"
#include "IvRendererDX11.h"
#include "IvResourceManagerDX11.h"
#include "IvShaderProgramDX11.h"
#include "IvVertexBufferDX11.h"
#include "IvUniform.h"

#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static D3D_PRIMITIVE_TOPOLOGY sPrimTypeMap[kPrimTypeCount];

static IvShaderProgramDX11* sDefaultShaders[kVertexFormatCount];

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
IvRendererDX11::Create(ID3D11Device* device, ID3D11DeviceContext* context,
                       ID3D11RenderTargetView* renderTarget, 
					   ID3D11DepthStencilView* depthStencilTarget)
{
	if (!mRenderer)
	{
		mRenderer = new IvRendererDX11(device, context, renderTarget, depthStencilTarget);
	}
    return ( mRenderer != 0 );

}   // End of IvRenderer::Create()


//-------------------------------------------------------------------------------
// @ IvRendererDX11::IvRendererDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvRendererDX11::IvRendererDX11(ID3D11Device* device, ID3D11DeviceContext* context,
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

	mAPI = kDX11;

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

}   // End of IvRendererDX11::IvRendererDX11()


//-------------------------------------------------------------------------------
// @ IvRendererDX11::~IvRendererDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvRendererDX11::~IvRendererDX11()
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

/*    if (mResourceManager)
    {
        delete (IvResourceManagerDX11*)(mResourceManager);
        mResourceManager = 0;
    }*/

	mDepthStencilTarget->Release();
	mDepthStencilTarget = 0;

	mRenderTarget->Release();
	mRenderTarget = 0;

	mContext->Release();
	mContext = 0;

	mDevice->Release();
	mDevice = 0;

}   // End of IvRendererDX11::~IvRendererDX11()


//-------------------------------------------------------------------------------
// @ IvRendererDX11::Initialize()
//-------------------------------------------------------------------------------
// Initialize display
//-------------------------------------------------------------------------------
bool
IvRendererDX11::Initialize( unsigned int width, unsigned int height )
{
    if (!InitDX11())
        return false;

    // set parameters for window size
    Resize( width, height );
    
    // create resource manager
//    mResourceManager = new IvResourceManagerDX11( mDevice );

    return true;
    
}   // End of IvRendererDX11::Initialize()
    

//-------------------------------------------------------------------------------
// @ IvRendererDX11::Resize()
//-------------------------------------------------------------------------------
// Set up parameters for window size
//-------------------------------------------------------------------------------
void 
IvRendererDX11::Resize(unsigned int width, unsigned int height ) 
{
	/*
	// prevent divide by zero
    if (height == 0)                                    
    {
        height = 1;                                 
    }

    // set up current viewport
	D3DVIEWPORT9 viewPort = { 0, 0, width, height, 0.0f, 1.0f };
    mDevice->SetViewport( &viewPort );                     
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
*/
}   // End of IvRendererDX11::Resize()


//-------------------------------------------------------------------------------
// @ IvRendererDX11::InitGL()
//-------------------------------------------------------------------------------
// Set up base GL parameters
//-------------------------------------------------------------------------------
bool 
IvRendererDX11::InitDX11()                                   
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
	rasterDesc.FrontCounterClockwise = false;
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
    // turn on culling
	mDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

    // set point size
    float pointSize = 5.0f;
    mDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&pointSize) );
	*/
    return true;                                        

}   // End of IvRendererDX11::InitGL()


//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetClearColor()
//-------------------------------------------------------------------------------
// Set background color
//-------------------------------------------------------------------------------
void 
IvRendererDX11::SetClearColor( float red, float green, float blue, float alpha )  
{
    // set clear color
	mClearColor[0] = red;
	mClearColor[1] = green;
	mClearColor[2] = blue;
	mClearColor[3] = alpha;
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetClearDepth()
//-------------------------------------------------------------------------------
// Set base depth value
//-------------------------------------------------------------------------------
void 
IvRendererDX11::SetClearDepth( float depth )  
{
    // set clear depth
    mClearDepth = depth; 
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::ClearBuffers()
//-------------------------------------------------------------------------------
// Clear necessary buffers
//-------------------------------------------------------------------------------
void 
IvRendererDX11::ClearBuffers(IvClearBuffer buffer)
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
// @ IvRendererDX11::SetBlendFunc()
//-------------------------------------------------------------------------------
// Set the pixel-blending function
//-------------------------------------------------------------------------------
void 
IvRendererDX11::SetBlendFunc(IvBlendFunc blend)
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
// @ IvRendererDX11::SetColorMask()
//-------------------------------------------------------------------------------
// Set which colors will actually be written to the color buffer
//-------------------------------------------------------------------------------
void IvRendererDX11::SetColorMask( bool red, bool green, bool blue, bool alpha )
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
// @ IvRendererDX11::SetFillMode()
//-------------------------------------------------------------------------------
// Set whether we're in solid or wireframe drawing mode
//-------------------------------------------------------------------------------
void IvRendererDX11::SetFillMode( IvFillMode fill )
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
// @ IvRendererDX11::SetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
void IvRendererDX11::SetShadeMode(IvShadeMode shade)
{
	/*
    if (shade == kFlatShaded)
        mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
    else if (shade == kGouraudShaded)
        mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	*/
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::GetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
IvShadeMode 
IvRendererDX11::GetShadeMode()
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
		ASSERT(false);
		return kGouraudShaded;
	}
	
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetDepthTest()
//-------------------------------------------------------------------------------
// Set the depth-testing function
//-------------------------------------------------------------------------------
void IvRendererDX11::SetDepthTest(IvDepthTestFunc func)
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
// @ IvRendererDX11::GetDepthTest()
//-------------------------------------------------------------------------------
// Get the depth-testing function
//-------------------------------------------------------------------------------
IvDepthTestFunc IvRendererDX11::GetDepthTest()
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
// @ IvRendererDX11::SetDepthWrite()
//-------------------------------------------------------------------------------
// Enable/Disable the writing of depths to the depth buffer
//-------------------------------------------------------------------------------
void IvRendererDX11::SetDepthWrite(bool write)
{
//	mDevice->SetRenderState( D3DRS_ZWRITEENABLE, write ? TRUE : FALSE );
}

//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetWorldMatrix()
//-------------------------------------------------------------------------------
// Sets the world matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererDX11::SetWorldMatrix(const IvMatrix44& matrix)
{
 //   IvRenderer::SetWorldMatrix(matrix);

	//mDevice->SetTransform( D3DTS_WORLDMATRIX(0), (const D3DMATRIX *)&mWorldMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetViewMatrix()
//-------------------------------------------------------------------------------
// Sets the camera matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererDX11::SetViewMatrix(const IvMatrix44& matrix)
{
 //   IvRenderer::SetViewMatrix(matrix);

	//mDevice->SetTransform( D3DTS_VIEW, (const D3DMATRIX *)&mViewMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetProjectionMatrix()
//-------------------------------------------------------------------------------
// Sets the projection matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererDX11::SetProjectionMatrix(const IvMatrix44& matrix)
{
 //   IvRenderer::SetProjectionMatrix(matrix);

	//mDevice->SetTransform( D3DTS_PROJECTION, (const D3DMATRIX *)&mProjectionMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererDX11::GetShaderProgram()
//-------------------------------------------------------------------------------
// Returns the currently-active shader program
//-------------------------------------------------------------------------------
IvShaderProgram* IvRendererDX11::GetShaderProgram()
{
    return mShader;
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::SetShaderProgram()
//-------------------------------------------------------------------------------
// Applies a shader program
//-------------------------------------------------------------------------------
void IvRendererDX11::SetShaderProgram(IvShaderProgram* program)
{
    // This is a safe cast, since we will only link IvShaderProgramDX11 with the
    // DX11 renderer.
    //mShader = static_cast<IvShaderProgramDX11*>(program);
    //if (mShader)
    //{
    //    mShader->MakeActive( mDevice );
    //}
    //else
    //{
    //    // Otherwise (NULL shader), we will need to bind a default shader later 
	   // mDevice->SetVertexShader( 0 );
	   // mDevice->SetPixelShader( 0 );
    //}
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererDX11::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
                  IvIndexBuffer* indexBuffer, unsigned int numIndices)
{
 //   BindDefaultShaderIfNeeded(vertexBuffer->GetVertexFormat());

	//// update any default uniforms
	//if ( mShader )
	//{
	//	IvUniform* modelviewproj = mShader->GetUniform("IvModelViewProjectionMatrix");
	//	if ( modelviewproj )
	//	{
	//		modelviewproj->SetValue( mWVPMat, 0 );
	//	}
	//	IvUniform* normalMat = mShader->GetUniform("IvNormalMatrix");
	//	if ( normalMat )
	//	{
	//		normalMat->SetValue( mNormalMat, 0 );
	//	}
	//	IvUniform* diffuseColor = mShader->GetUniform("IvDiffuseColor");
	//	if (diffuseColor)
	//	{
	//		diffuseColor->SetValue(mDiffuseColor,0);
	//	}
	//	IvUniform* ambient = mShader->GetUniform("IvLightAmbient");
	//	if ( ambient )
	//	{
	//		ambient->SetValue(mLightAmbient,0);
	//	}
	//	IvUniform* diffuse = mShader->GetUniform("IvLightDiffuse");
	//	if ( diffuse )
	//	{
	//		diffuse->SetValue(mLightDiffuse,0);
	//	}
	//	IvUniform* direction = mShader->GetUniform("IvLightDirection");
	//	if ( direction )
	//	{
	//		direction->SetValue(mLightDirection,0);
	//	}
	//}

 //   if (vertexBuffer)
 //       static_cast<IvVertexBufferDX11*>(vertexBuffer)->MakeActive( mDevice );
 //   else
 //       return;

 //   if (indexBuffer)
 //       static_cast<IvIndexBufferDX11*>(indexBuffer)->MakeActive( mDevice );
 //   else
 //       return;

	//int primCount = 0;
	//switch ( primType )
	//{
	//case kLineListPrim:
	//	primCount = numIndices/2;
	//	break;

	//case kLineStripPrim:
	//	primCount = numIndices-1;
	//	break;

	//case kTriangleListPrim:
	//	primCount = numIndices/3;
	//	break;

	//case kTriangleStripPrim:
	//	primCount = numIndices-2;
	//	break;
	//}

	//if ( primCount < 1 )
	//	return;

 //   mDevice->DrawIndexedPrimitive( sPrimTypeMap[primType], 0, 0, vertexBuffer->GetVertexCount(), 
	//	0, primCount );
}

//-------------------------------------------------------------------------------
// @ IvRendererDX11::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererDX11::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices)
{
 //   BindDefaultShaderIfNeeded(vertexBuffer->GetVertexFormat());

	//// update any default uniforms
	//if ( mShader )
	//{
	//	IvUniform* modelviewproj = mShader->GetUniform("IvModelViewProjectionMatrix");
	//	if ( modelviewproj )
	//	{
	//		modelviewproj->SetValue(mWVPMat, 0);
	//	}
	//	IvUniform* normalMat = mShader->GetUniform("IvNormalMatrix");
	//	if ( normalMat )
	//	{
	//		normalMat->SetValue(mNormalMat, 0);
	//	}
	//	IvUniform* diffuseColor = mShader->GetUniform("IvDiffuseColor");
	//	if (diffuseColor)
	//	{
	//		diffuseColor->SetValue(mDiffuseColor,0);
	//	}
	//	IvUniform* ambient = mShader->GetUniform("IvLightAmbient");
	//	if ( ambient )
	//	{
	//		ambient->SetValue(mLightAmbient,0);
	//	}
	//	IvUniform* diffuse = mShader->GetUniform("IvLightDiffuse");
	//	if ( diffuse )
	//	{
	//		diffuse->SetValue(mLightDiffuse,0);
	//	}
	//	IvUniform* direction = mShader->GetUniform("IvLightDirection");
	//	if ( direction )
	//	{
	//		direction->SetValue(mLightDirection,0);
	//	}
	//}

 //   if (vertexBuffer)
 //       static_cast<IvVertexBufferDX11*>(vertexBuffer)->MakeActive( mDevice );
 //   else
 //       return;

	//int primCount = 0;
	//switch ( primType )
	//{
	//case kPointListPrim:
	//	primCount = numVertices;
	//	break;

	//case kLineListPrim:
	//	primCount = numVertices/2;
	//	break;

	//case kLineStripPrim:
	//	primCount = numVertices-1;
	//	break;

	//case kTriangleListPrim:
	//	primCount = numVertices/3;
	//	break;

	//case kTriangleStripPrim:
	//	primCount = numVertices-2;
	//	break;

	//if ( primCount < 1 )
	//	return;

 //   mDevice->DrawPrimitive( sPrimTypeMap[primType], 0, primCount );
}


//-------------------------------------------------------------------------------
// @ IvRendererDX11::BindDefaultShaderIfNeeded()
//-------------------------------------------------------------------------------
// Selects a default shader for the given vertex format if none is bound
//-------------------------------------------------------------------------------
void IvRendererDX11::BindDefaultShaderIfNeeded(IvVertexFormat format)
{
/*    if (mShader)
        return;

    if (!sDefaultShaders[format])
    {
        IvVertexShader* vs = mResourceManager->CreateDefaultVertexShader(format); 
        IvFragmentShader* fs = mResourceManager->CreateDefaultFragmentShader(format);

        sDefaultShaders[format] = static_cast<IvShaderProgramDX11*>(
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

    SetShaderProgram(sDefaultShaders[format]);*/
}
        