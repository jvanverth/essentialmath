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
// Builds an object database and then updates it.  Used for collision demo.
// See ObjectDB.cpp for more details.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvClock.h>

#include "Game.h"
#include "ObjectDB.h"
#include "CollisionObject.h"

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
    mObjectDB = 0;          // main player object

}   // End of Game::Game()


//-------------------------------------------------------------------------------
// @ Game::~Game()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Game::~Game()
{
    delete mObjectDB;
    mObjectDB = 0;
    
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
    IvGame::PostRendererInitialize();
    
    // initialize random number generator
    srand( GetTime() );

    mObjectDB = new ObjectDB();
    if (!mObjectDB || !mObjectDB->Initialize(10) )
        return false;

    // add a bunch of objects
    int i;
    for (i = 0; i < 8; ++i)
    {
        CollisionObject* object = new CollisionObject;
        mObjectDB->AddObject( object );
    }

    // set up lighting
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
    // update objects
    mObjectDB->Update( dt );
    
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  // Here's Where We Do All The Drawing
{   
    // look at the origin of the world 
    IvSetDefaultViewer( 0.0f, -5.0f, 0.0f );
    
    // draw the objects
    mObjectDB->Render();

    // draw the box
    IvRenderer::mRenderer->SetFillMode( kWireframeFill );

    IvSetWorldIdentity();
    // draw wireframe cube
    IvDrawLine( IvVector3( 1.0f, 1.0f, 1.0f ), IvVector3( 1.0f, 1.0f, -1.0f ) );
    IvDrawLine( IvVector3( 1.0f, 1.0f, -1.0f ), IvVector3( 1.0f, -1.0f, -1.0f ) );
    IvDrawLine( IvVector3( 1.0f, -1.0f, -1.0f ), IvVector3( 1.0f, -1.0f, 1.0f ) );
    IvDrawLine( IvVector3( 1.0f, -1.0f, 1.0f ), IvVector3( 1.0f, 1.0f, 1.0f ) );

    IvDrawLine( IvVector3( -1.0f, 1.0f, 1.0f ), IvVector3( -1.0f, 1.0f, -1.0f ) );
    IvDrawLine( IvVector3( -1.0f, 1.0f, -1.0f ), IvVector3( -1.0f, -1.0f, -1.0f ) );
    IvDrawLine( IvVector3( -1.0f, -1.0f, -1.0f ), IvVector3( -1.0f, -1.0f, 1.0f ) );
    IvDrawLine( IvVector3( -1.0f, -1.0f, 1.0f ), IvVector3( -1.0f, 1.0f, 1.0f ) );

    IvDrawLine( IvVector3( 1.0f, 1.0f, 1.0f ), IvVector3( -1.0f, 1.0f, 1.0f ) );
    IvDrawLine( IvVector3( 1.0f, 1.0f, -1.0f ), IvVector3( -1.0f, 1.0f, -1.0f ) );
    IvDrawLine( IvVector3( 1.0f, -1.0f, -1.0f ), IvVector3( -1.0f, -1.0f, -1.0f ) );
    IvDrawLine( IvVector3( 1.0f, -1.0f, 1.0f ), IvVector3( -1.0f, -1.0f, 1.0f ) );
    
//    IvDrawCube( IvWhite );

    IvRenderer::mRenderer->SetFillMode( kSolidFill );

}   // End of Game::Render()

