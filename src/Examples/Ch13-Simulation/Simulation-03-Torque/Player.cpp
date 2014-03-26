//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This example demonstrates force-based simulation code, including both linear 
// and angular factors.  In this case, we are using Euler's method to integrate
// two sets of equations, one for forces as before, and one for torques.
//
// The key commands are:
//
// I,J,K,L: generate forces in the XY direction to push the Player object around.
// SPACE:   resets demo
// ESC, Q:  quit
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

static const IvVector3 kForceOffset( 1.0f, 1.0f, 1.0f );

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
    mRotate.Identity();
    mTranslate.Zero();

    mVelocity.Zero();
    mMass = 10.0f;
    mConstantForce.Zero();

    // moments of inverse for 2x2x2 box
    mMomentsInverse.Identity();
    mMomentsInverse(0,0) = 1.0f/12.0f*mMass*(2.0f*2.0f + 2.0f*2.0f);
    mMomentsInverse(1,1) = 1.0f/12.0f*mMass*(2.0f*2.0f + 2.0f*2.0f);
    mMomentsInverse(2,2) = 1.0f/12.0f*mMass*(2.0f*2.0f + 2.0f*2.0f);
    mMomentsInverse.Inverse();
    mAngularMomentum.Zero();
    mAngularVelocity.Zero();
    mConstantTorque.Zero();

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
    float x = 0.0f, y = 0.0f, z = 0.0f; 
    
    // where the force is applied
    IvVector3 offset = mRotate.Rotate( kForceOffset );
    IvVector3 force;

    // add up part of forces and torques which are constant over interval
    mConstantForce.Zero();
    mConstantTorque.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        force = -50.0f*IvVector3::xAxis;
        mConstantForce += force;
        mConstantTorque += offset.Cross( force );

    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        force = 50.0f*IvVector3::xAxis;
        mConstantForce += force;
        mConstantTorque += offset.Cross( force );
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        force = -50.0f*IvVector3::yAxis;
        mConstantForce += force;
        mConstantTorque += offset.Cross( force );
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        force = 50.0f*IvVector3::yAxis;
        mConstantForce += force;
        mConstantTorque += offset.Cross( force );
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
        mTranslate.Zero();
        mVelocity.Zero();
        mAngularVelocity.Zero();
        mAngularMomentum.Zero();
    }

    // move to next time step
    Integrate( dt );

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::Integrate()
//-------------------------------------------------------------------------------
// Move to next time step, using Euler's method
//-------------------------------------------------------------------------------
void 
Player::Integrate( float dt )                                   
{   
    // compute acceleration
    IvVector3 accel = 1.0f/mMass * CurrentForce( mTranslate, mVelocity, 
                                                 mRotate, mAngularVelocity );
    // clear small values
    accel.Clean();          

    // compute new velocity, position
    mVelocity += dt*accel;
    // clear small values
    mVelocity.Clean();      
    mTranslate += dt*mVelocity;
    
    IvVector3 torque = CurrentTorque( mTranslate, mVelocity, mRotate, mAngularVelocity );

    // compute new angular momentum, orientation
    mAngularMomentum += dt*torque;
    mAngularMomentum.Clean();
    
    // update angular velocity
    IvMatrix33 rotateMat(mRotate);
    IvMatrix33 worldMomentsInverse = rotateMat*mMomentsInverse*::Transpose(rotateMat);
    mAngularVelocity = worldMomentsInverse*mAngularMomentum;
    mAngularVelocity.Clean();

    IvQuat w(mAngularVelocity);
    mRotate += 0.5f*dt*(w*mRotate);
    mRotate.Normalize();
    mRotate.Clean();

}   // End of Player::Integrate()


//-------------------------------------------------------------------------------
// @ Player::CurrentForce()
//-------------------------------------------------------------------------------
// Compute forces, given forces constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
Player::CurrentForce( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalForce = mConstantForce;
    // add some drag
    float speed = velocity.Length();
    totalForce -= 1.5f*mMass*speed*velocity;

    return totalForce;

}   // End of Player::CurrentForce()


//-------------------------------------------------------------------------------
// @ Player::CurrentTorque()
//-------------------------------------------------------------------------------
// Compute torque, given torques constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
Player::CurrentTorque( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalTorque = mConstantTorque;
    // fake some angular friction -- should use moments of inertia, really
    float speed = angularVelocity.Length();
    totalTorque -= 0.5f*mMass*speed*angularVelocity;    

    return totalTorque;

}   // End of Player::CurrentTorque()


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
    transform.Identity();
    (void) transform.Rotation( mRotate );
    transform(0,3) = mTranslate.GetX();
    transform(1,3) = mTranslate.GetY();
    transform(2,3) = mTranslate.GetZ();
    
    // draw the object
    IvSetWorldMatrix(transform);
    IvDrawCube( kYellow );
                    
    // need to rotate force vector into local space and add to offset to get end
    IvVector3 forceEnd = kForceOffset + ::Conjugate(mRotate).Rotate(mConstantForce);
    IvDrawLine( kForceOffset, forceEnd, kOrange );
}

