//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This is the main player object.  It is derived from SimObject, so all the 
// integration and collision detection code can be found there.  All this does
// is set up forces to push the "player" around so it can hit the "obstacle".
//
// The key commands are:
//
// I,J,K,L: generate forces in the XY direction to push the Player object around.
// SPACE:   resets demo
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvEventHandler.h>
#include <IvMath.h>

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
Player::Player() : SimObject()
{
    SetTranslate( IvVector3( 0.0f, -12.0f, 0.0f ) );
    mElasticity = 0.75f;

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
    // add up forces, torques
    mConstantForce.Zero();
    mConstantTorque.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        mConstantForce -= 36.0f*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        mConstantForce += 36.0f*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        mConstantForce -= 36.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        mConstantForce += 36.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        SetTranslate( IvVector3( 0.0f, -12.0f, 0.0f ) );
        mRotate.Set( IvVector3::zAxis, kHalfPI );
        mVelocity.Zero();
        mAngularMomentum.Zero();
        mAngularVelocity.Zero();
    }

    SimObject::Update( dt );
}
