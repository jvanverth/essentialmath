//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This example demonstrates the three basic transforms and how they interact
// with each other.   At each frame we append transforms to the current matrix,
// which is stored for the next frame.  The key commands are:
//
// I, K - translate in x
// J, L - translate in y
// U, O - rotate around z axis
// P, : - uniformly scale
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvVector3.h>
#include <IvRendererHelp.h>

#include "Player.h"
#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Player::Player()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Player::Player()
{
}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    // get change in transform for this frame
    IvMatrix44 scale, rotate, xlate;
    scale.Identity();
    rotate.Identity();
    float s = 1.0f;
    float r = 0.0f;
    float x = 0.0f, y = 0.0f, z = 0.0f; 
    
    // set up scaling
    if (IvGame::mGame->mEventHandler->IsKeyDown(';'))
    {
        s -= 0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('p'))
    {
        s += 0.25f*dt;
    }
    scale.Scaling( IvVector3(s, s, s) );
    
    // set up rotate
    if (IvGame::mGame->mEventHandler->IsKeyDown('o'))
    {
        r -= kPI*0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('u'))
    {
        r += kPI*0.25f*dt;
    }
    rotate.RotationZ( r );
    
    // set up translation
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        x -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        x += 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        y -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        y += 3.0f*dt;
    }
    IvVector3 xlatevector(x,y,z);
    xlate.Translation( xlatevector );
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyPressed(' '))
    {
        mTransform.Identity();
    }

    // append transforms for this frame to current transform
    // note order: mTransform is applied first, then scale, then rotate, then xlate
    mTransform = xlate*rotate*scale*mTransform;
    
}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    // store current matrix
    IvRenderer::mRenderer->SetWorldMatrix( mTransform );

    // draw geometry
    IvDrawTeapot();
}
