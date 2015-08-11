//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Game::Orthographic() creates a orthographic projection matrix and sets it in 
// the rendering pipeline.  It does this based on the locations of a view window
// and the near and far planes.
//
// You can toggle between the orthographic and perspective projections (using
// Frustum(), which has a similar interface) and see the differences.  Note how 
// the view window affects how much is rendered depending on the projection, and 
// the different effects on parallel lines into the distance. 
//
// The key commands are:
//
// w, s - move near plane out and in
// e, d - move far plane out and in
// i, k - move view window up and down
// j, l - move view window left and right
// p - toggle between orthographic and perspective projection
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
// Needs to be here so that IvGame::mGame variable can be created in IvMain.cpp
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
    // init variables
    mXCenter = 0.0f;
    mYCenter = 0.0f;

    mUseOrthographic = true;


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

    IvRenderer::mRenderer->SetNearPlane(3.5f);
    IvRenderer::mRenderer->SetFarPlane(25.0f);

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
    // set near plane
    if (mEventHandler->IsKeyDown('w'))
    {
        IvRenderer::mRenderer->SetNearPlane( IvRenderer::mRenderer->GetNearPlane() + dt );
    }
    if (mEventHandler->IsKeyDown('s'))
    {
        IvRenderer::mRenderer->SetNearPlane( IvRenderer::mRenderer->GetNearPlane() - dt );
    }
    
    // set far plane
    if (mEventHandler->IsKeyDown('e'))
    {
        IvRenderer::mRenderer->SetFarPlane( IvRenderer::mRenderer->GetFarPlane() + dt );
    }
    if (mEventHandler->IsKeyDown('d'))
    {
        IvRenderer::mRenderer->SetFarPlane( IvRenderer::mRenderer->GetFarPlane() - dt );
    }

    // set x position of window
    if (mEventHandler->IsKeyDown('j'))
    {
        mXCenter -= 0.75f*dt;
    }
    if (mEventHandler->IsKeyDown('l'))
    {
        mXCenter += 0.75f*dt;
    }
    
    // set y position of window
    if (mEventHandler->IsKeyDown('i'))
    {
        mYCenter += 0.75f*dt;
    }
    if (mEventHandler->IsKeyDown('k'))
    {
        mYCenter -= 0.75f*dt;
    }

    // toggle projection
    if (mEventHandler->IsKeyPressed('p'))
    {
        mUseOrthographic = !mUseOrthographic;
    }

    // reset
    if (mEventHandler->IsKeyDown(' '))
    {
        IvRenderer::mRenderer->SetFOV( 45.0f );
        IvRenderer::mRenderer->SetNearPlane( 3.5f );
        IvRenderer::mRenderer->SetFarPlane(25.0f);
        mXCenter = 0.0f;
        mYCenter = 0.0f;
    }
        
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Orthographic()
//-------------------------------------------------------------------------------
// Set orthographic projection matrix
//-------------------------------------------------------------------------------
void 
Game::Orthographic( float left, float right, 
               float bottom, float top,
               float nearZ, float farZ )
{
    IvMatrix44 ortho;
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float recipZ = 1.0f/(nearZ-farZ);

        ortho(0,0) = 2.0f*recipX;
        ortho(0,3) = -(right+left)*recipX;

        ortho(1,1) = 2.0f*recipY;
        ortho(1,3) = -(top+bottom)*recipY;

        ortho(2,2) = 2.0f*recipZ;
        ortho(2,3) = (nearZ+farZ)*recipZ;
    }
    else
    {
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float recipZ = 1.0f/(farZ-nearZ);

        ortho(0,0) = 2.0f*recipX;
        ortho(0,3) = -(right+left)*recipX;

        ortho(1,1) = 2.0f*recipY;
        ortho(1,3) = -(top+bottom)*recipY;

        ortho(2,2) = recipZ;
        ortho(2,3) = -nearZ*recipZ;
    }

    // send to renderer
    ::IvSetProjectionMatrix( ortho );
}


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
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float Q = farZ/(farZ-nearZ);

        perspective(0,0) = 2.0f*nearZ*recipX;
        perspective(0,2) = -(right+left)*recipX;

        perspective(1,1) = 2.0f*nearZ*recipY;
        perspective(1,2) = -(top+bottom)*recipY;

        perspective(2,2) = Q;
        perspective(2,3) = -nearZ*Q;

        perspective(3,2) = 1.0f;
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
    // set perspective matrix
    float aspect = (float)IvRenderer::mRenderer->GetWidth()/(float)IvRenderer::mRenderer->GetHeight();
    if ( mUseOrthographic )
    {
        Orthographic( -aspect*3.0f+mXCenter, aspect*3.0f+mXCenter,
                   -3.0f+mYCenter, 3.0f+mYCenter,
                    IvRenderer::mRenderer->GetNearPlane(), 
                    IvRenderer::mRenderer->GetFarPlane() );
    }
    else
    {
        Frustum( -aspect*3.0f+mXCenter, aspect*3.0f+mXCenter,
                   -3.0f+mYCenter, 3.0f+mYCenter,
                    IvRenderer::mRenderer->GetNearPlane(), 
                    IvRenderer::mRenderer->GetFarPlane() );
    }

    // set viewer
    ::IvSetDefaultViewer( 6.0f, 0.0f, 5.f );

    // draw axes
    ::IvDrawAxes();

    // draw floor
    ::IvDrawFloor();

    // draw some objects
    IvMatrix44 worldTransform;
    worldTransform.Translation( IvVector3( 0.1f, 0.0f, 1.0f ) );
    ::IvSetWorldMatrix( worldTransform );
    ::IvDrawCube();

    worldTransform.Translation( IvVector3( 0.1f, 3.0f, 0.0f ) );
    ::IvSetWorldMatrix( worldTransform );
    ::IvDrawTeapot();

    worldTransform.Translation( IvVector3( 0.1f, -2.5f, 1.0f ) );
    ::IvSetWorldMatrix( worldTransform );
    ::IvDrawSphere( 1.0f );

}
