//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This example demonstrates some simple tank code.  The tank can be driven 
// forward, backward, turned, and turned in place.  The orange lines indicate
// the relative forces (they've been scaled down) used to drive the tank.
//
// The key commands are:
//
// I,K: move tank forward, back
// J,L: turn tank left, right
// SPACE:   resets demo
// ESC:  quit
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvAssert.h>
#include <IvRenderer.h>
#include <IvFileReader.h>
#include <IvEventHandler.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>

#include "Game.h"
#include "Player.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static const IvVector3 kForceOffset1( 0.0f, 1.0f, 0.0f );
static const IvVector3 kForceOffset2( 0.0f, -1.0f, 0.0f );

const int kTankIndex = 0;
const int kTurretIndex = 1;
const int kBarrelIndex = 2;

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
    mScale = 1.0f;
    mTranslate.Zero();

    mVelocity.Zero();
    mMass = 61325.688424f;  // yeah, it's massive
    mForce1.Zero();
    mForce2.Zero();
    mConstantForce.Zero();

    // moments of inverse for box the size of a tank
    float lengthsq = 9.7536f*9.7536f;
    float widthsq = 3.6576f*3.6576f;
    float heightsq = 2.4384f*2.4384f;
    mMomentsInverse.Identity();
    mMomentsInverse(0,0) = mMass*(widthsq + heightsq)/12.0f;
    mMomentsInverse(1,1) = mMass*(lengthsq + heightsq)/12.0f;
    mMomentsInverse(2,2) = mMass*(widthsq + lengthsq)/12.0f;
    mMomentsInverse.Inverse();
    mAngularMomentum.Zero();
    mAngularVelocity.Zero();
    mConstantTorque.Zero();

    // Build the tank hierarchy, which looks like:
    //               Tank 
    //                |           
    //              Turret
    //                |             
    //              Barrel

    bool result;
    result = mTank.AllocNodes(3);
    ASSERT(result);

    // Read the tank body geometry from file
    IvFileReader tankFile("Tank.txt");
    result = mTank.AddNode(kTankIndex, kTankIndex, tankFile,
                           IvVector3::origin, IvQuat(), 0.2f);
    ASSERT(result);

    // Read the tank turret geometry from file
    IvFileReader turretFile("Turret.txt");
    result = mTank.AddNode(kTurretIndex, kTankIndex, turretFile,
                           IvVector3(0.75f, 0.0f, 3.55f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the tank barrel geometry from file
    IvFileReader barrelFile("Barrel.txt");
    result = mTank.AddNode(kBarrelIndex, kTurretIndex, barrelFile,
                           IvVector3(4.5f, 0.0f, 1.0f), IvQuat(), 1.0f);
    ASSERT(result);

    // Update the transforms and bounds
    mTank.UpdateWorldTransforms();

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
    mForce1.Zero();
    mForce2.Zero();
    IvVector3 forwardVector = mRotate.Rotate(IvVector3::xAxis);
    IvVector3 sideVector = mRotate.Rotate(IvVector3::yAxis);

    // if moving forward
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        if (!IvGame::mGame->mEventHandler->IsKeyDown('j'))
        {
            mForce1 -= 6.0f*forwardVector;
        }
        else
        {
            mForce1 -= 4.0f*forwardVector;
        }
        if (!IvGame::mGame->mEventHandler->IsKeyDown('l'))
        {
            mForce2 -= 6.0f*forwardVector;
        }
        else
        {
            mForce2 -= 4.0f*forwardVector;
        }
    }
    // if moving backward
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        if (!IvGame::mGame->mEventHandler->IsKeyDown('j'))
        {
            mForce1 += 6.0f*forwardVector;
        }
        else
        {
            mForce1 += 4.0f*forwardVector;
        }
        if (!IvGame::mGame->mEventHandler->IsKeyDown('l'))
        {
            mForce2 += 6.0f*forwardVector;
        }
        else
        {
            mForce2 += 4.0f*forwardVector;
        }
    }
    // if no forward or backward movement
    if (!IvGame::mGame->mEventHandler->IsKeyDown('i') && !IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
        {
            mForce1 -= 3.0f*forwardVector;
            mForce2 += 3.0f*forwardVector;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
        {
            mForce1 += 3.0f*forwardVector;
            mForce2 -= 3.0f*forwardVector;
        }
    }
    // boost those forces to move that massive thing
    mForce1 *= 100000.0f;
    mForce2 *= 100000.0f;

    // clear values
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
        mTranslate.Zero();
        mVelocity.Zero();

        mAngularMomentum.Zero();
        mAngularVelocity.Zero();
    }

    // add up part of forces and torques which are constant over interval
    mConstantForce = mForce1+mForce2;

    // where the force is applied
    IvVector3 offset = mRotate.Rotate( kForceOffset1 );
    // compute torque
    mConstantTorque = offset.Cross( mForce1 );
    offset = mRotate.Rotate( kForceOffset2 );
    mConstantTorque += offset.Cross( mForce2 );

    // compute new physical values
    Integrate( dt );

    mTank.SetLocalTranslate( mTranslate, 0 );
    mTank.SetLocalRotate( mRotate, 0 );
    mTank.UpdateWorldTransforms();
    
}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    
    mTank.Render();
                
    // draw force line
    IvSetWorldIdentity();

    // need to rotate force vector into local space and add to offset to get end
    IvVector3 offset = mRotate.Rotate(kForceOffset1) + mTranslate;
    IvVector3 forceEnd = offset + mForce1/100000.0f;
    IvDrawLine( offset, forceEnd, kOrange );
    offset = mRotate.Rotate(kForceOffset2) + mTranslate;
    forceEnd = offset + mForce2/100000.0f;
    IvDrawLine( offset, forceEnd, kOrange );
}


//-------------------------------------------------------------------------------
// @ Player::Integrate()
//-------------------------------------------------------------------------------
// Move to next time step
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
    mRotate.Clean();
    mRotate.Normalize();

}   // End of Player::Integrate()


//-------------------------------------------------------------------------------
// @ Player::CurrentForce()
//-------------------------------------------------------------------------------
// Compute torque, given forces constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
Player::CurrentForce( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalForce = mConstantForce;
    // add some drag
    float speed = velocity.Length();
    totalForce -= 7.5f*mMass*speed*velocity;

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
    totalTorque -= 200.0f*mMass*speed*angularVelocity;  

    return totalTorque;

}   // End of Player::CurrentTorque()


