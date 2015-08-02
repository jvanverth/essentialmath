//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// Game::Oblique() creates an oblique parallel matrix and sets it in the 
// rendering pipeline.  It does this based on the window location on the near
// plane and locations of the near and far planes.
//
// Game::Cabinet() creates an alternative oblique parallel matrix, the effect of
// which is to foreshorten distances parallel to the near plane to half their
// length.
//
// The key commands are:
//
// w, s - move near plane out and in
// e, d - move far plane out and in
// i, k - move view window up and down
// j, l - move view window left and right
// p - switch projections
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
    // init variables
    mXCenter = 0.0f;
    mYCenter = 0.0f;
    mUseCabinet = false;

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
    
    IvSetDefaultLighting();

    IvRenderer::mRenderer->SetNearPlane( 0.1f );
    IvRenderer::mRenderer->SetFarPlane( 15.0f );

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
    if (mEventHandler->IsKeyDown('p'))
    {
        mUseCabinet = !mUseCabinet;
        mEventHandler->KeyUp('p');
    }

    // reset
    if (mEventHandler->IsKeyDown(' '))
    {
        IvRenderer::mRenderer->SetNearPlane( 0.1f );
        IvRenderer::mRenderer->SetFarPlane( 15.0f );
        mXCenter = 0.0f;
        mYCenter = 0.0f;
    }
        
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Oblique()
//-------------------------------------------------------------------------------
// Set one type of oblique parallel projection matrix.
// Note that this is slightly different than the book version. This one is set up 
// so that x and y shear in the positive direction, so the effect is that we're 
// looking down on the objects.
//-------------------------------------------------------------------------------
void 
Game::Oblique( float left, float right, 
               float bottom, float top,
               float nearZ, float farZ )
{
    IvMatrix44 oblique;
    oblique.Identity();
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float recipZ = 1.0f/(nearZ-farZ);

        oblique(0,0) = 2.0f*recipX;
        oblique(0,2) = -recipX;
        oblique(0,3) = -(right+left)*recipX - nearZ*recipX;

        oblique(1,1) = 2.0f*recipY;
        oblique(1,2) = -recipY;
        oblique(1,3) = -(top+bottom)*recipY - nearZ*recipY;

        oblique(2,2) = 2.0f*recipZ;
        oblique(2,3) = (nearZ+farZ)*recipZ;
    }
    else
    {
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float recipZ = 1.0f/(farZ-nearZ);

        oblique(0,0) = 2.0f*recipX;
        oblique(0,2) = recipX;
        oblique(0,3) = -(right+left)*recipX - nearZ*recipX;

        oblique(1,1) = 2.0f*recipY;
        oblique(1,2) = recipY;
        oblique(1,3) = -(top+bottom)*recipY - nearZ*recipY;

        oblique(2,2) = recipZ;
        oblique(2,3) = -nearZ*recipZ;
    }

    // send to renderer
    IvSetProjectionMatrix( oblique );
}


//-------------------------------------------------------------------------------
// @ Game::Cabinet()
//-------------------------------------------------------------------------------
// Set cabinet projection matrix.
// This is an alternative oblique projection that has a projection angle of 
// cot(theta/2) to the projection plane (instead of cot(theta/2) in both the x 
// and y directions, as the above one does).  The effect is that distances 
// parallel to the view direction foreshorten to half their length.
// This is a 45 degree cabinet projection, since it shears equally in both x and y
//-------------------------------------------------------------------------------
void 
Game::Cabinet( float left, float right, 
               float bottom, float top,
               float nearZ, float farZ )
{
    IvMatrix44 cabinet;
    cabinet.Identity();
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float recipZ = 1.0f/(nearZ-farZ);

        float halfsqrt2 = ::IvSqrt(2.0f)*0.5f;

        cabinet(0,0) = 2.0f*recipX;
        cabinet(0,2) = -halfsqrt2*recipX;
        cabinet(0,3) = (-halfsqrt2*nearZ - right - left)*recipX; 

        cabinet(1,1) = 2.0f*recipY;
        cabinet(1,2) = -halfsqrt2*recipY;
        cabinet(1,3) = (-halfsqrt2*nearZ - top - bottom)*recipY;

        cabinet(2,2) = 2.0f*recipZ;
        cabinet(2,3) = (nearZ+farZ)*recipZ;
    }
    else
    {
        float recipX = 1.0f/(right-left);
        float recipY = 1.0f/(top-bottom);
        float recipZ = 1.0f/(farZ-nearZ);

        float halfsqrt2 = ::IvSqrt(2.0f)*0.5f;

        cabinet(0,0) = 2.0f*recipX;
        cabinet(0,2) = halfsqrt2*recipX;
        cabinet(0,3) = (-halfsqrt2*nearZ - right - left)*recipX; 

        cabinet(1,1) = 2.0f*recipY;
        cabinet(1,2) = halfsqrt2*recipY;
        cabinet(1,3) = (-halfsqrt2*nearZ - top - bottom)*recipY;

        cabinet(2,2) = recipZ;
        cabinet(2,3) = -nearZ*recipZ;
    }

    // send to renderer
    IvSetProjectionMatrix( cabinet );
}


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  
{       
    const float viewerDistance = 6.0f;
    const float halfHeight = 3.0f;
    float quartersqrt2 = ::IvSqrt(2.0f)*0.25f;

    // set projection matrix
    float aspect = (float)IvRenderer::mRenderer->GetWidth()/(float)IvRenderer::mRenderer->GetHeight();

    if (mUseCabinet)
    {
        // note that we offset the window by quartersqrt2*viewerDistance so
        // we start with the origin in the center of the screen
        // this counteracts the normal shear of -sqrt(2)/4 in x and y for each unit of z
        Cabinet( -aspect*halfHeight + mXCenter + quartersqrt2*viewerDistance,
                 aspect*halfHeight + mXCenter + quartersqrt2*viewerDistance,
                -halfHeight + mYCenter + quartersqrt2*viewerDistance,
                 halfHeight + mYCenter + quartersqrt2*viewerDistance,
                IvRenderer::mRenderer->GetNearPlane(), 
                IvRenderer::mRenderer->GetFarPlane() );
    }
    else
    {
        // note that we offset the window by quartersqrt2*viewerDistance so
        // we start with the origin in the center of the screen
        // this counteracts the normal shear of -1/2 in x and y for each unit of z
        Oblique( -aspect*halfHeight + mXCenter + 0.5f*viewerDistance,
                 aspect*halfHeight + mXCenter + 0.5f*viewerDistance,
                -halfHeight + mYCenter + 0.5f*viewerDistance,
                 halfHeight + mYCenter + 0.5f*viewerDistance,
                IvRenderer::mRenderer->GetNearPlane(), 
                IvRenderer::mRenderer->GetFarPlane() );
    }

    // set viewer
    IvSetDefaultViewer( viewerDistance, 0.0f, 0.0f );

    // draw axes
    IvDrawAxes();

    // draw floor
    IvDrawFloor();

    // draw some objects
    IvMatrix44 worldTransform;
    worldTransform.Translation( IvVector3( 0.1f, 0.0f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawCube();

    worldTransform.Translation( IvVector3( 0.1f, 3.0f, 0.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawTeapot();

    worldTransform.Translation( IvVector3( 0.1f, -2.5f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawSphere( 1.0f );

}
