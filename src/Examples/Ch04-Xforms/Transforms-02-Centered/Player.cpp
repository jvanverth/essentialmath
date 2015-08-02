//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This example demonstrates the three basic transforms and how to achieve
// object-centered transformations.  At each frame we append or prepend 
// transforms to the current matrix, which is stored for the next frame.  
// The key commands are:
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
    IvMatrix44 scale, rotate, xlate, unxform, rexform;
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
    scale.Scaling(IvVector3(s, s, s));
    
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
    xlate.Translation( IvVector3(x, y, z) );
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mTransform.Identity();
    }

    // append transforms for this frame to current transform
    // note order: 
    // since scale is uniform in this example, we can apply first in transform chain
    // then we apply old transform
    // then translate center to origin, rotate around origin,
    // then retranslate to old position
    // then finally, apply new translation
    IvVector3 originalXlate( mTransform(0,3), mTransform(1,3), mTransform(2,3) );
    unxform.Translation( -originalXlate );
    rexform.Translation( originalXlate );
    mTransform = xlate*rexform*rotate*unxform*mTransform*scale;
    
    // alternatively, we can save some processing by clearing mTransform's translation
    // and adding originalXlate to the new translation.  The result would be
    // mTransform = xlate*rotate*mTransform*scale;
    
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
