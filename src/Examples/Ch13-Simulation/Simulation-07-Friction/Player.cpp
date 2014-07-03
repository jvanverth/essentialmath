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
#include "SimObject.h"

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
Player::Player() : mObject(0)
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
    // add up forces, torques
    IvVector3 constantForce;
    constantForce.Zero();
    IvVector3 constantTorque;
    constantTorque.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        constantForce -= 64.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        constantForce += 64.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        constantForce += 64.0f*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        constantForce -= 64.0f*IvVector3::xAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('u'))
    {
        constantTorque += 36.0f*IvVector3::zAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('o'))
    {
        constantTorque -= 36.0f*IvVector3::zAxis;
    }
    
    mObject->SetConstantForce( constantForce );
    mObject->SetConstantTorque( constantTorque );
    
}
