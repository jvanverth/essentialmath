//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvClock.h>
#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMatrix33.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>
#include <IvVector4.h>

#include "Game.h"
#include "Player.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvGame::Create()
//-------------------------------------------------------------------------------
// Creation routine
//-------------------------------------------------------------------------------
bool
IvGame::Create()
{
    IvGame::mGame = new Game();

    return true;
}   // End of IvGame::Create()

//-------------------------------------------------------------------------------
// @ Game::Game()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Game::Game()
{
    // init variables
    mPlayer = 0;            // main player object

}   // End of Game::Game()


//-------------------------------------------------------------------------------
// @ Game::~Game()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Game::~Game()
{
    delete mPlayer;
    mPlayer = 0;
    
}   // End of Game::~Game()


//-------------------------------------------------------------------------------
// @ Game::PostRendererInitialize()
//-------------------------------------------------------------------------------
// Initialize subsystems
//-------------------------------------------------------------------------------
bool
Game::PostRendererInitialize()
{
    if (!IvGame::PostRendererInitialize())
        return false;

    mPlayer = new Player();
    if (!mPlayer)
        return false;

    // plug in a light
    ::IvSetDefaultLighting();

    // Set The View
    ::IvSetDefaultViewer( -10.0f, 2.0f, 20.0f );

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
    // update player
    mPlayer->Update( mClock->GetElapsedTime() );
    
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  // Here's Where We Do All The Drawing
{
    // draw axes
    IvDrawAxes();

    // draw some teapots to look at
    IvMatrix44 transform;
    transform.Identity();
    transform(0,3) = 1.0f;
    transform(1,3) = 1.0f;
    transform(2,3) = 0.0f;
    IvSetWorldMatrix( transform );
    IvDrawTeapot();

    transform(0,3) = -1.0f;
    transform(1,3) = -1.0f;
    transform(2,3) = 6.0f;
    IvSetWorldMatrix( transform );
    IvDrawTeapot();

    transform(0,3) = 12.0f;
    transform(1,3) = 0.0f;
    transform(2,3) = 0.0f;
    IvSetWorldMatrix( transform );
    IvDrawTeapot();

    transform(0,3) = 6.0f;
    transform(1,3) = -1.0f;
    transform(2,3) = 0.0f;
    IvSetWorldMatrix( transform );
    IvDrawTeapot();

    transform.Identity();
    IvSetWorldMatrix( transform );

    // draw the main object
    mPlayer->Render();
}
