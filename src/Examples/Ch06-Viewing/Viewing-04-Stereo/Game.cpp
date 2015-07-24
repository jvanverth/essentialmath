//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This example demonstrates how to use oblique perspective projections to 
// create red-blue stereograms, i.e. images that appear 3D if you wear
// glasses with a blue left lens and a red right lens.  This is based on 
// an example by Paul Bourke, with modifications by Daniel van Vugt.
//
// The general concept is that each eye looks forward along the view direction, 
// separated by a distance, here called mEyeSeparation.  Their view frustums meet
// a single view window located mFocalLength away from the viewer.  The resulting 
// view frustums are not centered on the view direction, so an oblique perspective 
// projection has to be created, using the call Game::Frustum().  
//
// To achieve the red-blue separation, we render from the perspective of each eye,
// but only render with one color channel.  This is done by using the Renderer call
// SetColorMask().  It takes four arguments, and turns on or off the red, green,
// blue or alpha channels (see Chapter 7) depending on whether the argument is
// true or false.  For example, the call SetColorMask(true, false, false, true) 
// only stores the red and alpha values of the rasterization step into the color 
// buffer (see Chapter 9).
//
// The process is as follows: 
// 1) Use the color mask to render in only red 
// 2) Set up an oblique view frustum from the right eye's point of view
// 3) Set the view position to be the location of the right eye
// 4) Render the scene from the right eye's perspective
// 5) Clear only the z-buffer (see Chapter 9), leaving the red bits we just rendered.
//    We don't have to worry about overwriting them, since for the left eye we're 
//    only going to be rendering in blue.
// 6) Use the color mask to render in only blue
// 7) Set up an oblique view frustum from the left eye's point of view
// 8) Set the view position to be the location of the left eye
// 9) Render the scene from the left eye's perspective
//
// Note that in order for this to work well, all objects need to be grayscale. 
// Also, best results are achieved when mEyeSeparation is about 1/20th of
// mFocalLength.  More detailed information can be found at 
// http://astronomy.swin.edu.au/~pbourke/opengl/stereogl/.
//
// If you don't already have red-blue 3D glasses, you can find them on the web.
// One source is www.d3.com.  If you have red-green, or red-cyan glasses,
// you can use SetColorMask(false, true, false, true) for the green
// filter and SetColorMask(false, true, true, true) for the cyan 
// filter.
//
// The key commands are:
//
// i, k - move camera forward or backward
// j, l - turn camera left or right
// q, a - decrease, increase field of view
// w, s - move near plane out and in
// e, d - move far plane out and in
// space - reset to start
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <math.h>

#include <IvClock.h>
#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvMatrix33.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>
#include <IvVector4.h>

#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvGame::Create()
//-------------------------------------------------------------------------------
// Static constructor
//-------------------------------------------------------------------------------
bool
IvGame::Create()
{
    IvGame::mGame = new Game();
    return ( IvGame::mGame != 0 );

}   // End of IvGame::Create()


//-------------------------------------------------------------------------------
// @ Game::Game()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Game::Game() : IvGame()
{
    mFocalLength = 11.0f;
    mEyeSeparation = mFocalLength/20.0f;

    mViewPoint.Set( 6.0f, 5.0f, 5.f );
    // start off looking at origin
    mViewDirection = -mViewPoint;
    mViewDirection.Normalize();

}   // End of Game::Game()


//-------------------------------------------------------------------------------
// @ Game::~Game()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Game::~Game()
{
}   // End of Game::~Game()


//-------------------------------------------------------------------------------
// @ Game::PostRendererInitialize()
//-------------------------------------------------------------------------------
// Set up internal subsystems
//-------------------------------------------------------------------------------
bool 
Game::PostRendererInitialize()
{
    // Set up base class 
    if ( !IvGame::PostRendererInitialize() )
        return false;
    
    ::IvSetDefaultLighting();

    return true;
}   // End of Game::PostRendererInitialize()


//-------------------------------------------------------------------------------
// @ Game::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Game::UpdateObjects( float dt )
{
    // set field of view
    if (mEventHandler->IsKeyDown('q'))
    {
        IvRenderer::mRenderer->SetFOV( IvRenderer::mRenderer->GetFOV() - 0.1f );
    }
    if (mEventHandler->IsKeyDown('a'))
    {
        IvRenderer::mRenderer->SetFOV( IvRenderer::mRenderer->GetFOV() + 0.1f );
    }
    
    // set near plane
    if (mEventHandler->IsKeyDown('w'))
    {
        IvRenderer::mRenderer->SetNearPlane( IvRenderer::mRenderer->GetNearPlane() + 0.1f );
    }
    if (mEventHandler->IsKeyDown('s'))
    {
        IvRenderer::mRenderer->SetNearPlane( IvRenderer::mRenderer->GetNearPlane() - 0.1f );
    }
    
    // set far plane
    if (mEventHandler->IsKeyDown('d'))
    {
        IvRenderer::mRenderer->SetFarPlane( IvRenderer::mRenderer->GetFarPlane() - 0.1f );
    }
    if (mEventHandler->IsKeyDown('e'))
    {
        IvRenderer::mRenderer->SetFarPlane( IvRenderer::mRenderer->GetFarPlane() + 0.1f );
    }

    // change camera position
    float delta = 0.0f;
    if (mEventHandler->IsKeyDown('i'))
    {
        delta += dt;
    }
    if (mEventHandler->IsKeyDown('k'))
    {
        delta -= dt;
    }
    // move along view direction
    mViewPoint.x += 2.0f * mViewDirection.x * delta;
    mViewPoint.y += 2.0f * mViewDirection.y * delta;

    // change camera heading
    float headingChange = 0.0f;
    if (mEventHandler->IsKeyDown('j'))
    {
        headingChange += 0.25f*kPI*dt;
    }
    if (mEventHandler->IsKeyDown('l'))
    {
        headingChange -= 0.25f*kPI*dt;
    }
    float sintheta, costheta;
    IvSinCos(headingChange, sintheta, costheta);

    float x = mViewDirection.x*costheta - mViewDirection.y*sintheta;
    mViewDirection.y = mViewDirection.x*sintheta
                     + mViewDirection.y*costheta;
    mViewDirection.x =  x;
    // shouldn't need this, but it keeps floating precision issues under control
    mViewDirection.Normalize();

    // reset
    if (mEventHandler->IsKeyDown(' '))
    {
        mViewPoint.Set( 6.0f, 5.0f, 5.f );
        // start off looking at origin
        mViewDirection = -mViewPoint;
        mViewDirection.Normalize();
        IvRenderer::mRenderer->SetFOV( 45.0f );
        IvRenderer::mRenderer->SetNearPlane( 0.1f );
        IvRenderer::mRenderer->SetFarPlane( 27.5f );
    }
    
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::SetCamera()
//-------------------------------------------------------------------------------
// Set view transform for camera
// Assumes viewDir and viewSide are already normalized
// Note that we are using one view matrix for both OpenGL and D3D9 -- this affects
// how the perspective matrix is created, below.
//-------------------------------------------------------------------------------
void 
Game::SetCamera( const IvVector3& viewDir, 
                 const IvVector3& viewSide, 
                 const IvVector3& viewPos )
{
    // get the third view vector
    IvVector3 viewUp = viewSide.Cross( viewDir );

	// this effectively multiplies the view position by the
	// transpose of the view-to-world rotation and negates it
	IvVector3 invViewPos( -viewSide.Dot(viewPos), 
						  -viewUp.Dot(viewPos), 
						 viewDir.Dot(viewPos) );

	// build the matrix
	// the rows of the 3x3 are the view vectors
	// the y column is the view position rotated into view space
	IvMatrix44 viewMatrix;
	viewMatrix.Identity();
	viewMatrix(0,0) = viewSide.x;
	viewMatrix(0,1) = viewSide.y;
	viewMatrix(0,2) = viewSide.z;
	viewMatrix(0,3) = invViewPos.x;
	viewMatrix(1,0) = viewUp.x;
	viewMatrix(1,1) = viewUp.y;
	viewMatrix(1,2) = viewUp.z;
	viewMatrix(1,3) = invViewPos.y;
	viewMatrix(2,0) = -viewDir.x;
	viewMatrix(2,1) = -viewDir.y;
	viewMatrix(2,2) = -viewDir.z;
	viewMatrix(2,3) = invViewPos.z;

    ::IvSetViewMatrix( viewMatrix );

}   // End of Game::SetCamera()


//-------------------------------------------------------------------------------
// @ Game::Frustum()
//-------------------------------------------------------------------------------
// Set perspective projection matrix
//-------------------------------------------------------------------------------
void 
Game::Frustum( float left, float right, 
               float bottom, float top,
               float nearZ, float farZ )
{
    IvMatrix44 perspective;

	if (IvRenderer::mRenderer->GetAPI() == kOpenGL)
	{
		float recipX = 1.0f/(right-left);
		float recipY = 1.0f/(top-bottom);
		float recipZ = 1.0f/(nearZ-farZ);

		perspective(0,0) = 2.0f*nearZ*recipX;
		perspective(0,2) = (right+left)*recipX;

		perspective(1,1) = 2.0f*nearZ*recipY;
		perspective(1,2) = (top+bottom)*recipY;

		perspective(2,2) = (nearZ+farZ)*recipZ;
		perspective(2,3) = 2.0f*nearZ*farZ*recipZ;

		perspective(3,2) = -1.0f;
		perspective(3,3) = 0.0f;
	}
	else
	{
		// modified D3D9 matrix with view direction down -z
		float recipX = 1.0f/(right-left);
		float recipY = 1.0f/(top-bottom);
		float Q = farZ/(nearZ-farZ);

		perspective(0,0) = 2.0f*nearZ*recipX;
		perspective(0,2) = (right+left)*recipX;

		perspective(1,1) = 2.0f*nearZ*recipY;
		perspective(1,2) = (top+bottom)*recipY;

		perspective(2,2) = Q;
		perspective(2,3) = nearZ*Q;

		perspective(3,2) = -1.0f;
		perspective(3,3) = 0.0f;
	}

    // send to renderer
    ::IvSetProjectionMatrix( perspective );
}


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  
{       
    // set up variables
    float aspect  = (float)IvRenderer::mRenderer->GetWidth()/(float)IvRenderer::mRenderer->GetHeight();
    float halfFov = IvRenderer::mRenderer->GetFOV()*kPI/360.0f;
    float halfWindowHeight = IvRenderer::mRenderer->GetNearPlane()*IvTan(halfFov);
    float nearRatio  = IvRenderer::mRenderer->GetNearPlane() / mFocalLength;

    // get view side vector so we can separate the eye positions
    IvVector3 viewSide = mViewDirection.Cross( IvVector3::zAxis );
    viewSide.Normalize();

    // render only using red channel
	IvRenderer::mRenderer->SetColorMask( true, false, false, true );

    // set right eye perspective matrix
    // set window on near plane so that frustum will be incident at focal length
    float left = -aspect*halfWindowHeight - 0.5f*mEyeSeparation*nearRatio;
    float right = aspect*halfWindowHeight - 0.5f*mEyeSeparation*nearRatio;
    float top =  halfWindowHeight;
    float bottom = -halfWindowHeight;
    Frustum( left, right,
             bottom, top,
             IvRenderer::mRenderer->GetNearPlane(), 
             IvRenderer::mRenderer->GetFarPlane() );

    // set view transform for right eye
    SetCamera( mViewDirection, viewSide, mViewPoint + 0.5f*mEyeSeparation*viewSide );
       
    // draw what the right eye sees
    DrawScene();

    // clear only the z-buffer, leaving the red channel information
	IvRenderer::mRenderer->ClearBuffers( kDepthClear );
    // render only using blue channel
	IvRenderer::mRenderer->SetColorMask( false, false, true, true );

    // set left eye perspective matrix
    // translate window on near plane so that frustum will be incident at focal length
    left  += mEyeSeparation*nearRatio;
    right += mEyeSeparation*nearRatio;
    Frustum( left, right,
             bottom, top,
             IvRenderer::mRenderer->GetNearPlane(), 
             IvRenderer::mRenderer->GetFarPlane() );

    // set view transform for left eye
    SetCamera( mViewDirection, viewSide, mViewPoint - 0.5f*mEyeSeparation*viewSide );

    // draw what the left eye sees
    DrawScene();

    // reset to use all channels (so we can clear properly for next frame)
	IvRenderer::mRenderer->SetColorMask( true, true, true, true );
}


//-------------------------------------------------------------------------------
// @ Game::DrawScene()
//-------------------------------------------------------------------------------
// Draw everything in the scene
//-------------------------------------------------------------------------------
void 
Game::DrawScene()
{
    IvMatrix44 identity;
    IvSetWorldMatrix( identity );

    // draw floor
    IvDrawFloor();

    // draw some objects
    IvMatrix44 worldTransform;
    worldTransform.Translation( IvVector3( 0.0f, 0.0f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    // has to be grayscale!
    IvDrawCube( kWhite );

    worldTransform.Translation( IvVector3( 0.0f, 3.0f, 0.0f ) );
    IvSetWorldMatrix( worldTransform );
    // has to be grayscale!
    IvDrawTeapot( kWhite );

    worldTransform.Translation( IvVector3( 0.0f, -2.5f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    // has to be grayscale!
    IvDrawSphere( 1.0f, kWhite );

}

