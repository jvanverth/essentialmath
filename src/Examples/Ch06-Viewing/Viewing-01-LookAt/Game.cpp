//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// This demo shows how to set up a look-at view transformation.  The method
// Game::LookAt() creates the world-to-view matrix and sets it in the rendering
// pipeline.
//
// One thing to try: back up the object until the camera is looking right down
// on it.  If you manage it, the screen will go to black.  This happens because
// the cross product between the up vector and view vector goes to zero.
//
// The key commands are:
//
// i, k - translate teapot in x
// j, l - translate teapot in y
// space - reset to origin
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
    
    mObjectPosition.Set( 0.0f, 0.0f, 1.0f );

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
    // move view focus
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        mObjectPosition -= 3.0f*dt*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        mObjectPosition += 3.0f*dt*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        mObjectPosition -= 3.0f*dt*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        mObjectPosition += 3.0f*dt*IvVector3::yAxis;
    }

    // reset to origin
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mObjectPosition.Set( 0.0f, 0.0f, 1.0f );
    }
    
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::LookAt()
//-------------------------------------------------------------------------------
// Set view matrix
//-------------------------------------------------------------------------------
void 
Game::LookAt( const IvVector3& eye, const IvVector3& lookAt, const IvVector3& up )                              
{
    // compute view vectors
    IvVector3 view = lookAt - eye;
    view.Normalize();
    
    IvVector3 right =  view.Cross( up );
    right.Normalize();
    
    IvVector3 viewUp =  right.Cross( view );
    viewUp.Normalize();

    // now set up matrices
    // world->view rotation
    IvMatrix33 rotate;
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        rotate.SetRows( right, viewUp, -view );
    }
    else
    {
        rotate.SetRows( right, viewUp, view );
    }

    // world->view translation
    IvVector3 xlate = -(rotate*eye);

    // build 4x4 matrix
    IvMatrix44 matrix(rotate);
    matrix(0,3) = xlate.x;
    matrix(1,3) = xlate.y;
    matrix(2,3) = xlate.z;

    ::IvSetViewMatrix( matrix );
}


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  
{       
    // set viewer
    LookAt( IvVector3(-10.0f, 0.0f, 10.0f), mObjectPosition, IvVector3::zAxis );

    // draw axes
    IvDrawAxes();

    // draw floor
    IvDrawFloor();

    // draw the object
    IvMatrix44 xform;
    xform.Translation( mObjectPosition );
    IvSetWorldMatrix( xform );
    
    IvDrawTeapot();

}
