//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
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
    mTranslate.Set( -2.0f, -12.0f, 0.0f );
    mRotate.Set( IvVector3::zAxis, kHalfPI );

    SetBounds( 0.5f, 3.0f );
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
    // add up forces
    mConstantForce.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        mConstantForce -= 9.0f*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        mConstantForce += 9.0f*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        mConstantForce -= 9.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        mConstantForce += 9.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mTranslate.Set( -2.0f, -12.0f, 0.0f );
        mRotate.Set( IvVector3::zAxis, kHalfPI );
        mVelocity.Zero();
        mAngularMomentum.Zero();
        mAngularVelocity.Zero();
    }

    SimObject::Update( dt );
}
