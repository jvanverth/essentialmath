//===============================================================================
// @ IvRendererD3D9.cpp
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
#include "IvIndexBufferD3D9.h"
#include <IvMath.h>
#include "IvRendererD3D9.h"
#include "IvResourceManagerD3D9.h"
#include "IvShaderProgramD3D9.h"
#include "IvVertexBufferD3D9.h"
#include "IvUniform.h"

#include <d3d9.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static D3DPRIMITIVETYPE sPrimTypeMap[kPrimTypeCount];

static IvShaderProgramD3D9* sDefaultShaders[kVertexFormatCount];

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
IvRendererD3D9::Create( IDirect3DDevice9* device )
{
	if ( !mRenderer )
		mRenderer = new IvRendererD3D9( device );
    return ( mRenderer != 0 );

}   // End of IvRenderer::Create()


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::IvRendererD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvRendererD3D9::IvRendererD3D9( IDirect3DDevice9* device ) : IvRenderer()
{
	mDevice = device;
	mDevice->AddRef();

    mShader = NULL;

	mClearColor = 0xffffffff;
	mClearDepth = 1.0f;

	mAPI = kD3D9;

    sPrimTypeMap[kPointListPrim] = D3DPT_POINTLIST;
    sPrimTypeMap[kLineListPrim] = D3DPT_LINELIST;
    sPrimTypeMap[kLineStripPrim] = D3DPT_LINESTRIP;
    sPrimTypeMap[kTriangleListPrim] = D3DPT_TRIANGLELIST;
    sPrimTypeMap[kTriangleStripPrim] = D3DPT_TRIANGLESTRIP;
    sPrimTypeMap[kTriangleFanPrim] = D3DPT_TRIANGLEFAN;

    sBlendSrcFunc[kNoBlendFunc] = D3DBLEND_ONE;
    sBlendDestFunc[kNoBlendFunc] = D3DBLEND_ZERO;
    sBlendSrcFunc[kOpacityBlendFunc] = D3DBLEND_SRCALPHA;
    sBlendDestFunc[kOpacityBlendFunc] = D3DBLEND_INVSRCALPHA ;
    sBlendSrcFunc[kAddBlendFunc] = D3DBLEND_ONE;
    sBlendDestFunc[kAddBlendFunc] = D3DBLEND_ONE;
    sBlendSrcFunc[kMultiplyBlendFunc] = D3DBLEND_ZERO;
    sBlendDestFunc[kMultiplyBlendFunc] = D3DBLEND_SRCCOLOR;

    sDepthFunc[kDisableDepthTest] = D3DCMP_ALWAYS;
    sDepthFunc[kGreaterDepthTest] = D3DCMP_GREATER;
    sDepthFunc[kGreaterEqualDepthTest] = D3DCMP_GREATEREQUAL;
    sDepthFunc[kLessDepthTest] = D3DCMP_LESS;
    sDepthFunc[kLessEqualDepthTest] = D3DCMP_LESSEQUAL;

}   // End of IvRendererD3D9::IvRendererD3D9()


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::~IvRendererD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvRendererD3D9::~IvRendererD3D9()
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
        delete (IvResourceManagerD3D9*)(mResourceManager);
        mResourceManager = 0;
    }
	
	mDevice->Release();

}   // End of IvRendererD3D9::~IvRendererD3D9()
    

//-------------------------------------------------------------------------------
// @ IvRendererD3D9::Initialize()
//-------------------------------------------------------------------------------
// Initialize display
//-------------------------------------------------------------------------------
bool
IvRendererD3D9::Initialize( unsigned int width, unsigned int height )
{
    if (!InitD3D9())
        return false;

    // set GL parameters for window size
    Resize( width, height );
    
    // create resource manager
    mResourceManager = new IvResourceManagerD3D9( mDevice );

    return true;
    
}   // End of IvRendererD3D9::Initialize()
    
    
//-------------------------------------------------------------------------------
// @ IvRendererD3D9::Resize()
//-------------------------------------------------------------------------------
// Set up GL parameters for window size
//-------------------------------------------------------------------------------
void 
IvRendererD3D9::Resize(unsigned int width, unsigned int height ) 
{
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

}   // End of IvRendererD3D9::Resize()


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::InitGL()
//-------------------------------------------------------------------------------
// Set up base GL parameters
//-------------------------------------------------------------------------------
bool 
IvRendererD3D9::InitD3D9()                                   
{
    // turn on smooth shading
    mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );    
    
    // set clear color and depth
    SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);          
    SetClearDepth(1.0f); 
    
    // set up depth buffer
    SetDepthTest(kLessEqualDepthTest);
    SetDepthWrite(true);

    // turn on culling
	mDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

    // set point size
    float pointSize = 5.0f;
    mDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&pointSize) );

    return true;                                        

}   // End of IvRendererD3D9::InitGL()


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetClearColor()
//-------------------------------------------------------------------------------
// Set background color
//-------------------------------------------------------------------------------
void 
IvRendererD3D9::SetClearColor( float red, float green, float blue, float alpha )  
{
    // set clear color
	mClearColor = D3DCOLOR_COLORVALUE( red, green, blue, alpha );
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetClearColor()
//-------------------------------------------------------------------------------
// Set background color
//-------------------------------------------------------------------------------
void 
IvRendererD3D9::SetClearDepth( float depth )  
{
    // set clear depth
    mClearDepth = depth; 
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::ClearBuffers()
//-------------------------------------------------------------------------------
// Clear necessary buffers
//-------------------------------------------------------------------------------
void 
IvRendererD3D9::ClearBuffers(IvClearBuffer buffer)
{
    switch (buffer)
    {
        case kColorClear:
            mDevice->Clear(0, NULL, D3DCLEAR_TARGET, mClearColor, mClearDepth, 0 );
            break;
        case kDepthClear:
            mDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, mClearColor, mClearDepth, 0 );
            break;
        case kColorDepthClear:
            mDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, mClearColor, mClearDepth, 0 );
            break;
    };
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetBlendFunc()
//-------------------------------------------------------------------------------
// Set the pixel-blending function
//-------------------------------------------------------------------------------
void 
IvRendererD3D9::SetBlendFunc(IvBlendFunc blend)
{
    if (blend == kNoBlendFunc)
        mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    else
        mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	mDevice->SetRenderState( D3DRS_SRCBLEND, sBlendSrcFunc[blend] );
	mDevice->SetRenderState( D3DRS_DESTBLEND, sBlendDestFunc[blend] );
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetColorMask()
//-------------------------------------------------------------------------------
// Set which colors will actually be written to the color buffer
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetColorMask( bool red, bool green, bool blue, bool alpha )
{
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
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetFillMode()
//-------------------------------------------------------------------------------
// Set whether we're in solid or wireframe drawing mode
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetFillMode( IvFillMode fill )
{
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
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetShadeMode( IvShadeMode shade )
{
    if (shade == kFlatShaded)
        mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
    else if (shade == kGouraudShaded)
        mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::GetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
IvShadeMode 
IvRendererD3D9::GetShadeMode()
{
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
	else
	{
		ASSERT(false);
		return kGouraudShaded;
	}
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetDepthTest()
//-------------------------------------------------------------------------------
// Set the depth-testing function
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetDepthTest(IvDepthTestFunc func)
{
    if (func == kDisableDepthTest)
        mDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    else
        mDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	mDevice->SetRenderState( D3DRS_ZFUNC, sDepthFunc[func] );
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::GetDepthTest()
//-------------------------------------------------------------------------------
// Get the depth-testing function
//-------------------------------------------------------------------------------
IvDepthTestFunc IvRendererD3D9::GetDepthTest()
{
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

    ASSERT(false);
    return kLessEqualDepthTest;
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetDepthWrite()
//-------------------------------------------------------------------------------
// Enable/Disable the writing of depths to the depth buffer
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetDepthWrite(bool write)
{
	mDevice->SetRenderState( D3DRS_ZWRITEENABLE, write ? TRUE : FALSE );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetWorldMatrix()
//-------------------------------------------------------------------------------
// Sets the world matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetWorldMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetWorldMatrix(matrix);

	mDevice->SetTransform( D3DTS_WORLDMATRIX(0), (const D3DMATRIX *)&mWorldMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetViewMatrix()
//-------------------------------------------------------------------------------
// Sets the camera matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetViewMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetViewMatrix(matrix);

	mDevice->SetTransform( D3DTS_VIEW, (const D3DMATRIX *)&mViewMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetProjectionMatrix()
//-------------------------------------------------------------------------------
// Sets the projection matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetProjectionMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetProjectionMatrix(matrix);

	mDevice->SetTransform( D3DTS_PROJECTION, (const D3DMATRIX *)&mProjectionMat );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D9::GetShaderProgram()
//-------------------------------------------------------------------------------
// Returns the currently-active shader program
//-------------------------------------------------------------------------------
IvShaderProgram* IvRendererD3D9::GetShaderProgram()
{
    return mShader;
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::SetShaderProgram()
//-------------------------------------------------------------------------------
// Applies a shader program
//-------------------------------------------------------------------------------
void IvRendererD3D9::SetShaderProgram(IvShaderProgram* program)
{
    // This is a safe cast, since we will only link IvShaderProgramD3D9 with the
    // D3D9 renderer.
    mShader = static_cast<IvShaderProgramD3D9*>(program);
    if (mShader)
    {
        mShader->MakeActive( mDevice );
    }
    else
    {
        // Otherwise (NULL shader), we will need to bind a default shader later 
	    mDevice->SetVertexShader( 0 );
	    mDevice->SetPixelShader( 0 );
    }
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererD3D9::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
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

    if (vertexBuffer)
        static_cast<IvVertexBufferD3D9*>(vertexBuffer)->MakeActive( mDevice );
    else
        return;

    if (indexBuffer)
        static_cast<IvIndexBufferD3D9*>(indexBuffer)->MakeActive( mDevice );
    else
        return;

	int primCount = 0;
	switch ( primType )
	{
	case kLineListPrim:
		primCount = numIndices/2;
		break;

	case kLineStripPrim:
		primCount = numIndices-1;
		break;

	case kTriangleListPrim:
		primCount = numIndices/3;
		break;

	case kTriangleStripPrim:
		primCount = numIndices-2;
		break;
	}

	if ( primCount < 1 )
		return;

    mDevice->DrawIndexedPrimitive( sPrimTypeMap[primType], 0, 0, vertexBuffer->GetVertexCount(), 
		0, primCount );
}

//-------------------------------------------------------------------------------
// @ IvRendererD3D9::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererD3D9::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices)
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

    if (vertexBuffer)
        static_cast<IvVertexBufferD3D9*>(vertexBuffer)->MakeActive( mDevice );
    else
        return;

	int primCount = 0;
	switch ( primType )
	{
	case kPointListPrim:
		primCount = numVertices;
		break;

	case kLineListPrim:
		primCount = numVertices/2;
		break;

	case kLineStripPrim:
		primCount = numVertices-1;
		break;

	case kTriangleListPrim:
		primCount = numVertices/3;
		break;

	case kTriangleStripPrim:
		primCount = numVertices-2;
		break;

	case kTriangleFanPrim:
		primCount = numVertices-2;
		break;
	}

	if ( primCount < 1 )
		return;

    mDevice->DrawPrimitive( sPrimTypeMap[primType], 0, primCount );
}


//-------------------------------------------------------------------------------
// @ IvRendererD3D9::BindDefaultShaderIfNeeded()
//-------------------------------------------------------------------------------
// Selects a default shader for the given vertex format if none is bound
//-------------------------------------------------------------------------------
void IvRendererD3D9::BindDefaultShaderIfNeeded(IvVertexFormat format)
{
    if (mShader)
        return;

    if (!sDefaultShaders[format])
    {
        IvVertexShader* vs = mResourceManager->CreateDefaultVertexShader(format); 
        IvFragmentShader* fs = mResourceManager->CreateDefaultFragmentShader(format);

        sDefaultShaders[format] = static_cast<IvShaderProgramD3D9*>(
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
        