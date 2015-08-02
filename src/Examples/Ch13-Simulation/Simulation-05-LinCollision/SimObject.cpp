//===============================================================================
// @ SimObject.cpp
// ------------------------------------------------------------------------------
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This is the base class for simulation objects.  It handles integration of 
// linear and angular factors, and collision detection and response.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRendererHelp.h>
#include <IvMatrix44.h>
#include <IvMath.h>

#include "SimObject.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ SimObject::SimObject()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
SimObject::SimObject()
{
    mRotate.Identity();
    mTranslate.Zero();

    mMass = 5.0f;
    mElasticity = 0.5f;   // varies between 0 and 1
    mSphere.SetRadius( 1.0f );

    mVelocity.Zero();
    mConstantForce.Zero();
    mAngularMomentum.Zero();
    mAngularVelocity.Zero();
    mConstantTorque.Zero();

    InitializeMoments();

}   // End of SimObject::SimObject()


//-------------------------------------------------------------------------------
// @ SimObject::~SimObject()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
SimObject::~SimObject()
{
}   // End of SimObject::~SimObject()


//-------------------------------------------------------------------------------
// @ SimObject::SetMass()
//-------------------------------------------------------------------------------
// Set mass and reinitialize moments of inertia
//-------------------------------------------------------------------------------
void
SimObject::SetMass( float mass )
{
    mMass = mass;

    InitializeMoments();

}   // End of SimObject::SetBounds()


//-------------------------------------------------------------------------------
// @ SimObject::SetBounds()
//-------------------------------------------------------------------------------
// Set bounding data and reinitialize moments of inertia
//-------------------------------------------------------------------------------
void
SimObject::SetBounds( float radius )
{
    mSphere.SetRadius( radius );
    
    InitializeMoments();

}   // End of SimObject::SetBounds()


//-------------------------------------------------------------------------------
// @ SimObject::InitializeMoments()
//-------------------------------------------------------------------------------
// Initialize moments of inertia
//-------------------------------------------------------------------------------
void
SimObject::InitializeMoments()
{
    // set moments of inertia for sphere
    float radiussq = mSphere.GetRadius();
    radiussq *= radiussq;
    mMomentsInverse.Identity();
    mMomentsInverse(0,0) = 2.0f*mMass*radiussq/5.0f;
    mMomentsInverse(1,1) = 2.0f*mMass*radiussq/5.0f;
    mMomentsInverse(2,2) = 2.0f*mMass*radiussq/5.0f;
    mMomentsInverse.Inverse();

}   // End of SimObject::InitializeMoments()


//-------------------------------------------------------------------------------
// @ SimObject::SetTranslate()
//-------------------------------------------------------------------------------
// Set translation for object
//-------------------------------------------------------------------------------
void SimObject::SetTranslate( const IvVector3& position )
{
    mTranslate = position;
    mSphere.SetCenter( position );

}   // End of SimObject::SetTranslate()


//-------------------------------------------------------------------------------
// @ SimObject::Update()
//-------------------------------------------------------------------------------
// Update object
//-------------------------------------------------------------------------------
void
SimObject::Update( float dt )
{
    Integrate( dt );

    mSphere.SetCenter( mTranslate );

}   // End of SimObject::Update()


//-------------------------------------------------------------------------------
// @ SimObject::Integrate()
//-------------------------------------------------------------------------------
// Move to next time step
//-------------------------------------------------------------------------------
void 
SimObject::Integrate( float dt )                                    
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

}   // End of SimObject::Integrate()


//-------------------------------------------------------------------------------
// @ SimObject::CurrentForce()
//-------------------------------------------------------------------------------
// Compute torque, given forces constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
SimObject::CurrentForce( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalForce = mConstantForce;
    // add some drag
    totalForce -= 0.25f*mMass*velocity;

    return totalForce;

}   // End of SimObject::CurrentForce()


//-------------------------------------------------------------------------------
// @ SimObject::CurrentTorque()
//-------------------------------------------------------------------------------
// Compute torque, given torques constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
SimObject::CurrentTorque( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalTorque = mConstantTorque;
    // fake some angular friction -- should use moments of inertia, really
    totalTorque -= 0.25f*mMass*angularVelocity;

    return totalTorque;

}   // End of SimObject::CurrentTorque()


//-------------------------------------------------------------------------------
// @ SimObject::HandleCollision()
//-------------------------------------------------------------------------------
// Handle collisions between this object and another one
//-------------------------------------------------------------------------------
void
SimObject::HandleCollision( SimObject* other )
{
    // do sphere check
    IvVector3 collisionNormal, collisionPoint;
    float penetration;

    // if two objects are colliding
    if ( Colliding(other, collisionNormal, collisionPoint, penetration) )
    {
        // handle collision

        // push out by penetration
        mTranslate -= 0.5f*penetration*collisionNormal;
        other->mTranslate += 0.5f*penetration*collisionNormal;

        // compute relative velocity
        IvVector3 relativeVelocity = mVelocity - other->mVelocity;

        float vDotN = relativeVelocity.Dot( collisionNormal );
        if (vDotN < 0)
            return;

        // compute impulse factor
        float modifiedVel = vDotN/(1.0f/mMass + 1.0f/other->mMass);
        float j1 = -(1.0f+mElasticity)*modifiedVel;
        float j2 = (1.0f+other->mElasticity)*modifiedVel;

        // update velocities
        mVelocity += j1/mMass*collisionNormal;
        other->mVelocity += j2/other->mMass*collisionNormal;
    }

}   // End of SimObject::HandleCollision()


//-------------------------------------------------------------------------------
// @ SimObject::Colliding()
//-------------------------------------------------------------------------------
// Check to see if colliding with another SimObject
//-------------------------------------------------------------------------------
bool 
SimObject::Colliding( const SimObject* other, IvVector3& collisionNormal, 
                      IvVector3& collisionPoint, float& penetration ) const
{
    return mSphere.ComputeCollision( other->mSphere, collisionNormal, 
                              collisionPoint, penetration );

}   // End of SimObject::Colliding()


//-------------------------------------------------------------------------------
// @ SimObject::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
SimObject::Render()                                 
{   
    // build 4x4 matrix
    IvMatrix44 transform( mRotate );
    transform(0,3) = mTranslate.x;
    transform(1,3) = mTranslate.y;
    transform(2,3) = mTranslate.z;
    
    IvSetWorldMatrix( transform );
    IvDrawSphere( mSphere.GetRadius(), kCyan );
}

