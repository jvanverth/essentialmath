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
// This demo implements a very simple clipping routine.   A red triangle is 
// clipped against a plane, represented by yellow lines.  See IvClipper for more 
// details on how the clipping is handled.
//
// The key commands are:
//
// i, k - move triangle in and out
// j, l - move triangle left and right
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
#include <IvResourceManager.h>

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
Game::Game() : IvGame(), mPosition( 0.0f, -0.5f, 0.0f ), mPlaneBuffer(0)
{
    IvPlane clipPlane( 0.0f, -1.0f, 0.0f, 1.0f );
    mClipper.SetPlane( clipPlane );

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
    // set up triangle translation
    IvVector3 xlate;
    xlate.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        xlate -= 3.0f*dt*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        xlate += 3.0f*dt*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        xlate -= 3.0f*dt*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        xlate += 3.0f*dt*IvVector3::yAxis;
    }
    mPosition += xlate;
    
    // clear transforms
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mPosition.Set( 0.0f, -0.5f, 0.0f );
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
    // set viewer
    IvSetDefaultViewer( -10.0f, -2.0f, 5.f );

    // draw plane
    if ( !mPlaneBuffer )
    {
        mPlaneBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 44,
                                                                                       nullptr, kDefaultUsage);
        IvCPVertex* dataPtr = (IvCPVertex*) mPlaneBuffer->BeginLoadData();
        for ( float x = -10.0f; x <= 10.0f; x += 2.0f )
        {
            dataPtr->color = kYellow;
            dataPtr->position.Set(  x, 1.0f, 10.0f );
            ++dataPtr;

            dataPtr->color = kYellow;
            dataPtr->position.Set( x, 1.0f, -10.0f );
            ++dataPtr;
        }
        for ( float z = -10.0f; z <= 10.0f; z += 2.0f )
        {
            dataPtr->color = kYellow;
            dataPtr->position.Set( -10.0f, 1.0f, z );
            ++dataPtr;

            dataPtr->color = kYellow;
            dataPtr->position.Set( 10.0f, 1.0f, z );
            ++dataPtr;
        }
        (void) mPlaneBuffer->EndLoadData();
    }
    IvRenderer::mRenderer->Draw( kLineListPrim, mPlaneBuffer );

    // draw clipped triangle
    mClipper.StartClip();
    mClipper.ClipVertex( IvVector3( 1.0f, 1.0f, 2.0f )+mPosition );
    mClipper.ClipVertex( IvVector3( 0.0f, 3.0f, -2.0f )+mPosition );
    mClipper.ClipVertex( IvVector3( -1.0f, -3.0f, -1.0f )+mPosition );
    // duplicate first vertex (important)
    mClipper.ClipVertex( IvVector3( 1.0f, 1.0f, 2.0f )+mPosition );
    mClipper.FinishClip();

}
