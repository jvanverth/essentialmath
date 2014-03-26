//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// Game::Perspective() creates a perspective projection matrix and sets it in 
// the rendering pipeline.  It does this based on the field of view and locations
// of the near and far planes.
//
// Try setting the field of view very small or very large.  As it goes past 0 or 180, 
// the result is to invert the view.
//
// The key commands are:
//
// q, a - decrease, increase field of view
// w, s - move near plane out and in
// e, d - move far plane out and in
// space - reset to start
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

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
    
    // set lighting
    IvSetDefaultLighting();

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
        IvRenderer::mRenderer->SetFOV( IvRenderer::mRenderer->GetFOV() - dt*6.0f );
    }
    if (mEventHandler->IsKeyDown('a'))
    {
        IvRenderer::mRenderer->SetFOV( IvRenderer::mRenderer->GetFOV() + dt*6.0f );
    }
    
    // set near plane
    if (mEventHandler->IsKeyDown('w'))
    {
        IvRenderer::mRenderer->SetNearPlane( IvRenderer::mRenderer->GetNearPlane() + dt*6.0f );
    }
    if (mEventHandler->IsKeyDown('s'))
    {
        IvRenderer::mRenderer->SetNearPlane( IvRenderer::mRenderer->GetNearPlane() - dt*6.0f );
    }
    
    // set far plane
    if (mEventHandler->IsKeyDown('d'))
    {
        IvRenderer::mRenderer->SetFarPlane( IvRenderer::mRenderer->GetFarPlane() - dt*6.0f );
    }
    if (mEventHandler->IsKeyDown('e'))
    {
        IvRenderer::mRenderer->SetFarPlane( IvRenderer::mRenderer->GetFarPlane() + dt*6.0f );
    }

    if (mEventHandler->IsKeyDown(' '))
    {
        IvRenderer::mRenderer->SetFOV( 45.0f );
        IvRenderer::mRenderer->SetNearPlane( 0.1f );
        IvRenderer::mRenderer->SetFarPlane( 27.5f );
    }

}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Perspective()
//-------------------------------------------------------------------------------
// Set perspective projection matrix
//-------------------------------------------------------------------------------
void 
Game::Perspective( float fov, float aspect, float nearZ, float farZ )
{
	IvMatrix44 perspective;
	if (IvRenderer::mRenderer->GetAPI() == kOpenGL)
	{
		float d = 1.0f/IvTan(fov/180.0f*kPI*0.5f);
		float recip = 1.0f/(nearZ-farZ);

		perspective(0,0) = d/aspect;
		perspective(1,1) = d;
		perspective(2,2) = (nearZ+farZ)*recip;
		perspective(2,3) = 2.0f*nearZ*farZ*recip;
		perspective(3,2) = -1.0f;
		perspective(3,3) = 0.0f;
	}
	else
	{
		float d = 1.0f/IvTan(fov/180.0f*kPI*0.5f);
		float Q = farZ/(farZ-nearZ);

		perspective(0,0) = d/aspect;
		perspective(1,1) = d;
		perspective(2,2) = Q;
		perspective(2,3) = -nearZ*Q;
		perspective(3,2) = 1.0f;
		perspective(3,3) = 0.0f;
	}

    // send to Renderer
    IvSetProjectionMatrix( perspective );
}


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  
{       
    // set perspective matrix
    Perspective( IvRenderer::mRenderer->GetFOV(), 
                (float)IvRenderer::mRenderer->GetWidth()/(float)IvRenderer::mRenderer->GetHeight(), 
                IvRenderer::mRenderer->GetNearPlane(), 
                IvRenderer::mRenderer->GetFarPlane() );

    // set viewer
    IvSetDefaultViewer( -10.0f, 0.0f, 5.0f );

    // draw axes
    IvDrawAxes();

    // draw floor
    IvDrawFloor();

    // draw some objects
    IvMatrix44 worldTransform;
    worldTransform.Translation( IvVector3( 0.0f, 0.0f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawCube();

    worldTransform.Translation( IvVector3( 0.0f, 3.0f, 0.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawTeapot();

    worldTransform.Translation( IvVector3( 0.0f, -2.5f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawSphere( 1.0f );
}
