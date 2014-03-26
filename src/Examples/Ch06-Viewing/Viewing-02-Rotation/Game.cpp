//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// Game::Render() creates the world-to-view matrix and sets it in the rendering
// pipeline.  It does this based on the current rotation and position of the camera
// which are set by the viewer.
//
// The key commands are:
//
// i, k - rotate camera up and down
// j, l - rotate camera left and right
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
    mCameraPosition.Set( -10.0f, 0.0f, 10.0f );
    mCameraRotation.RotationY( kPI*0.125f );

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
    // rotate around local z
    float heading = 0.0f;
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        heading += kPI*0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        heading -= kPI*0.25f*dt;
    }
    IvMatrix33 headingRot;
    headingRot.RotationZ( heading );

    // rotate around local y
    float pitch = 0.0f;
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        pitch += kPI*0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        pitch -= kPI*0.25f*dt;
    }
    IvMatrix33 pitchRot;
    pitchRot.RotationY( pitch );

    // prepend to get rotation around local axes (Euler angles)
    mCameraRotation = mCameraRotation*pitchRot*headingRot;

    // reset camera
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mCameraRotation.RotationY( kPI*0.125f );
    }
    
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  
{       
    // adjust rotation to match view space axes
    // (in real application, store static version of initOrient matrix)
    IvMatrix33 initOrient;
    initOrient.Identity();
    initOrient(0,0) = 0.0f;
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        initOrient(0,2) = -1.0f;
    }
    else
    {
        initOrient(0,2) = 1.0f;
    }
    initOrient(1,0) = -1.0f;
    initOrient(1,1) = 0.0f;
    initOrient(2,1) = 1.0f;
    initOrient(2,2) = 0.0f;
    IvMatrix33 viewToWorldRot = mCameraRotation*initOrient;

    // set view matrix

    // world to view rotation is transpose of view to world
    IvMatrix44 matrix(::Transpose(viewToWorldRot));
    // world to view translation is -eye position times world to view rotation
    // or can multiply by transpose of view to world
    IvVector3 eyeInverse = -mCameraPosition*viewToWorldRot;
    // set translation of matrix
    matrix(0,3) = eyeInverse.GetX();
    matrix(1,3) = eyeInverse.GetY();
    matrix(2,3) = eyeInverse.GetZ();
    IvSetViewMatrix( matrix );

    // draw axes
    IvDrawAxes();

    // draw floor
    IvDrawFloor();

    // draw something semi-interesting to look at
    IvMatrix44 worldTransform;
    worldTransform.Translation( IvVector3( 0.0f, 0.0f, 1.0f ) );
    IvSetWorldMatrix( worldTransform );
    IvDrawTeapot();

}
