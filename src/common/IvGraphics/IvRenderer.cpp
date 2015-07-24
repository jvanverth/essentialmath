//===============================================================================
// @ IvRenderer.cpp
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

#include "IvRenderer.h"
#include "IvMatrix33.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

IvRenderer* IvRenderer::mRenderer = 0;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvRenderer::IvRenderer()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvRenderer::IvRenderer() : 
    mIsActive( true )
{
    mWidth = 0;
    mHeight = 0;

    mFOV = 45.0f;
    mNear = 0.1f;
    mFar = 35.0f;

	mDiffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	mLightAmbient.Set(1.0f, 1.0f, 1.0f, 1.0f);
	mLightDiffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
    mLightDirection = IvVector4::xAxis;

}   // End of IvRenderer::IvRenderer()


//-------------------------------------------------------------------------------
// @ IvRenderer::~IvRenderer()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvRenderer::~IvRenderer()
{
    
}   // End of IvRenderer::~IvRenderer()
    

//-------------------------------------------------------------------------------
// @ IvRenderer::Destroy()
//-------------------------------------------------------------------------------
// Shutdown function
//-------------------------------------------------------------------------------
void 
IvRenderer::Destroy()
{
	delete mRenderer;
	mRenderer = 0;
}   // End of IvRenderer::Destroy()


//-------------------------------------------------------------------------------
// @ IvRenderer::Initialize()
//-------------------------------------------------------------------------------
// Initialize display
//-------------------------------------------------------------------------------
bool
IvRenderer::Initialize( unsigned int width, unsigned int height )
{
    return true;
}   // End of IvRenderer::Initialize()
    
//-------------------------------------------------------------------------------
// @ IvRenderer::GetResourceManager()
//-------------------------------------------------------------------------------
// Returns the resource manager
//-------------------------------------------------------------------------------
const IvResourceManager* IvRenderer::GetResourceManager() const
{
    return mResourceManager;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::GetResourceManager()
//-------------------------------------------------------------------------------
// Returns the resource manager
//-------------------------------------------------------------------------------
IvResourceManager* IvRenderer::GetResourceManager()
{
    return mResourceManager;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::GetWorldMatrix()
//-------------------------------------------------------------------------------
// Gets the world matrix for the renderer
//-------------------------------------------------------------------------------
const IvMatrix44& IvRenderer::GetWorldMatrix()
{
    return mWorldMat;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetWorldMatrix()
//-------------------------------------------------------------------------------
// Sets the world matrix for the renderer
//-------------------------------------------------------------------------------
void IvRenderer::SetWorldMatrix(const IvMatrix44& matrix)
{
    mWorldMat = matrix;
    mWVPMat = mProjectionMat*mViewMat*mWorldMat;
	IvMatrix33 worldMat3x3;
	IvVector3 col0(mWorldMat(0,0), mWorldMat(1,0), mWorldMat(2,0));
	IvVector3 col1(mWorldMat(0,1), mWorldMat(1,1), mWorldMat(2,1));
	IvVector3 col2(mWorldMat(0,2), mWorldMat(1,2), mWorldMat(2,2));
	worldMat3x3.SetColumns(col0, col1, col2);
	mNormalMat.Rotation(Transpose(Inverse(worldMat3x3)));
}

//-------------------------------------------------------------------------------
// @ IvRenderer::GetViewMatrix()
//-------------------------------------------------------------------------------
// Gets the camera matrix for the renderer
//-------------------------------------------------------------------------------
const IvMatrix44& IvRenderer::GetViewMatrix()
{
    return mViewMat;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetViewMatrix()
//-------------------------------------------------------------------------------
// Sets the camera matrix for the renderer
//-------------------------------------------------------------------------------
void IvRenderer::SetViewMatrix(const IvMatrix44& matrix)
{
    mViewMat = matrix;
    mWVPMat = mProjectionMat*mViewMat*mWorldMat;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::GetProjectionMatrix()
//-------------------------------------------------------------------------------
// Gets the projection matrix for the renderer
//-------------------------------------------------------------------------------
const IvMatrix44& IvRenderer::GetProjectionMatrix()
{
    return mProjectionMat;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetProjectionMatrix()
//-------------------------------------------------------------------------------
// Sets the projection matrix for the renderer
//-------------------------------------------------------------------------------
void IvRenderer::SetProjectionMatrix(const IvMatrix44& matrix)
{
    mProjectionMat = matrix;
    mWVPMat = mProjectionMat*mViewMat*mWorldMat;
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetMaterialDiffuse()
//-------------------------------------------------------------------------------
// Set material diffuse color (can be overridden by shader)
//-------------------------------------------------------------------------------
void
IvRenderer::SetDefaultDiffuseColor(float red, float green, float blue, float alpha)
{
    mDiffuseColor.Set(red, green, blue, alpha);
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetDefaultLightAmbient()
//-------------------------------------------------------------------------------
// Sets ambient color value for base light
//-------------------------------------------------------------------------------
void
IvRenderer::SetDefaultLightAmbient(float red, float green, float blue, float alpha)
{
    mLightAmbient.Set(red, green, blue, alpha);
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetDefaultLightDiffuse()
//-------------------------------------------------------------------------------
// Sets diffuse color value for base light
//-------------------------------------------------------------------------------
void
IvRenderer::SetDefaultLightDiffuse(float red, float green, float blue, float alpha)
{
    mLightDiffuse.Set(red, green, blue, alpha);
}

//-------------------------------------------------------------------------------
// @ IvRenderer::SetDefaultLightDirection()
//-------------------------------------------------------------------------------
// Sets direction for base light
//-------------------------------------------------------------------------------
void
IvRenderer::SetDefaultLightDirection(const IvVector3& direction)
{
    mLightDirection.Set(direction.x, direction.y, direction.z, 0.0);
}
