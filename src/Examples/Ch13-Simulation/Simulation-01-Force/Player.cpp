//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This demo implements a simple physical simulation using only linear forces.
// The cyan sphere is being pulled towards the origin by a spring.  It is also 
// being affected by gravity and a slight drag force, as well as forces created
// by the user.  
//
// The key commands are:
//
// I,J,K,L: generate forces in the yz directions to pull the sphere around.
// W,S: increase and decrease the spring force
// SPACE: reset
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvEventHandler.h>
#include <IvMatrix44.h>
#include <IvDebugger.h>

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
    mTranslate.Zero();

    mVelocity.Zero();
    mMass = 10.0f;
    mConstantForce.Zero();

    mSpringConstant = 100.0f;

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
    // add up part of forces and torques which are constant over interval
    mConstantForce.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        mConstantForce -= 75.0f*IvVector3::zAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        mConstantForce += 75.0f*IvVector3::zAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        mConstantForce -= 75.0f*IvVector3::yAxis;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        mConstantForce += 75.0f*IvVector3::yAxis;
    }

    // adjust spring force
    if (IvGame::mGame->mEventHandler->IsKeyDown('w'))
    {
        mSpringConstant += 10.0f;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('s'))
    {
        mSpringConstant -= 10.0f;
    }

    // reset simulation
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mTranslate.Zero();
        mVelocity.Zero();
    }

    // integrate (via symplectic Euler)

    // compute acceleration
    IvVector3 accel = CurrentForce( mTranslate, mVelocity ) / mMass;
    // clear small values
    accel.Clean();          

    // compute new position, velocity
    mVelocity += dt*accel;
    // clear small values
    mVelocity.Clean();  
    mTranslate += dt*mVelocity;

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::CurrentForce()
//-------------------------------------------------------------------------------
// Compute force, given forces constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
Player::CurrentForce( const IvVector3& position, const IvVector3& velocity )                                
{   
    IvVector3 totalForce = mConstantForce;

    // add in spring force
    totalForce -= mSpringConstant*position;

    // add in gravity
    totalForce -= 9.8f*mMass*IvVector3::zAxis;

    // add some drag
    float speed = velocity.Length();
    totalForce -= 0.1f*mMass*speed*velocity;

    return totalForce;

}   // End of Player::CurrentForce()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    // build 4x4 matrix
    IvMatrix44 transform;
    transform.Translation(mTranslate);
    
    IvSetWorldMatrix(transform);
    IvDrawSphere(0.5f, kCyan );
                    
    // draw force line
    IvSetWorldMatrix(transform);
    IvDrawLine( IvVector3::origin, mConstantForce, kOrange );

    // draw line from origin to ball, to show spring
    IvSetWorldIdentity();
    IvDrawLine( IvVector3::origin, mTranslate, kWhite );

}

