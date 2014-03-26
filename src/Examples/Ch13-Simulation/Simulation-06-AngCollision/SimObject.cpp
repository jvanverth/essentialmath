//===============================================================================
// @ SimObject.cpp
// ------------------------------------------------------------------------------
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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
#include "IvAssert.h"

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

    mVelocity.Zero();
    mConstantForce.Zero();
    mAngularMomentum.Zero();
    mAngularVelocity.Zero();
    mConstantTorque.Zero();

    // set parameters that affect moments of inertia
    mMass = 2.0f;
    SetBounds( 1.0f, 2.0f );
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
SimObject::SetBounds( float radius, float length )
{
    mLocalCapsule.SetRadius( radius );
    length *= 0.5f;
    mLocalCapsule.SetSegment( IvLineSegment3( length*IvVector3::xAxis, -length*IvVector3::xAxis ) );

    mBoundingRadius = length+radius;

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
    // create moments of inertia for cylinder aligned along x axis, centered on origin
    float length = mLocalCapsule.Length();
    float radiussq = mLocalCapsule.GetRadius();
    radiussq *= radiussq;
    mMomentsInverse.Identity();
    mMomentsInverse(0,0) = 1.0f/0.5f*mMass*radiussq;
    mMomentsInverse(1,1) = 1.0f/0.25f*mMass*radiussq+ mMass*length*length/12.0f;
    mMomentsInverse(2,2) = 1.0f/mMomentsInverse(1, 1);

    mWorldMomentsInverse = mMomentsInverse;

}   // End of SimObject::InitializeMoments()


//-------------------------------------------------------------------------------
// @ SimObject::Update()
//-------------------------------------------------------------------------------
// Update object
//-------------------------------------------------------------------------------
void
SimObject::Update( float dt )
{
    Integrate( dt );

    // recompute world segment location
    mWorldCapsule = mLocalCapsule.Transform( 1.0f, mRotate, mTranslate );

    float radius = mWorldCapsule.GetRadius();
    ASSERT( radius > 0.0f );

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
    mWorldMomentsInverse = rotateMat*mMomentsInverse*::Transpose(rotateMat);
    mAngularVelocity = mWorldMomentsInverse*mAngularMomentum;
    mAngularVelocity.Clean();

    IvQuat w(mAngularVelocity);
    mRotate += 0.5f*dt*(w*mRotate);
    mRotate.Clean();
    mRotate.Normalize();

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
    IvVector3 collisionNormal, collisionPoint;
    float penetration;

    // if the two objects are colliding
    if (Colliding(other, collisionNormal, collisionPoint, penetration))
    {
        // push out by penetration, scaled by relative masses
        float relativeMass = mMass/(mMass + other->mMass);
        mTranslate -= (1.0f-relativeMass)*penetration*collisionNormal;
        other->mTranslate += relativeMass*penetration*collisionNormal;

        // compute relative velocity
        IvVector3 r1 = collisionPoint - mTranslate;
        IvVector3 r2 = collisionPoint - other->mTranslate;
        IvVector3 vel1 = mVelocity + Cross( mAngularVelocity, r1 );
        IvVector3 vel2 = other->mVelocity + Cross( other->mAngularVelocity, r2 );
        IvVector3 relativeVelocity = vel1 - vel2;

        // determine if we're heading away from each other
        float vDotN = relativeVelocity.Dot( collisionNormal );
        if (vDotN < 0)
            return;

        // compute impulse factor
        float denominator = (1.0f/mMass + 1.0f/other->mMass)*(collisionNormal.Dot(collisionNormal));

        // compute angular factors
        IvVector3 cross1 = Cross(r1, collisionNormal);
        IvVector3 cross2 = Cross(r2, collisionNormal);
        cross1 = mWorldMomentsInverse*cross1;
        cross2 = other->mWorldMomentsInverse*cross2;
        IvVector3 sum = Cross(cross1, r1) + Cross(cross2, r2);
        denominator += (sum.Dot(collisionNormal));

        // compute j
        float modifiedVel = vDotN/denominator;
        float j1 = -(1.0f+mElasticity)*modifiedVel;
        float j2 = (1.0f+mElasticity)*modifiedVel;

        // update velocities
        mVelocity += j1/mMass*collisionNormal;
        other->mVelocity += j2/other->mMass*collisionNormal;

        // update angular velocities
        mAngularMomentum += Cross(r1, j1*collisionNormal);
        mAngularVelocity = mWorldMomentsInverse*mAngularMomentum;
        other->mAngularMomentum += Cross(r2, j2*collisionNormal);
        other->mAngularVelocity = other->mWorldMomentsInverse*other->mAngularMomentum;
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
    // first check sphere collision (cheaper)
    float radiusSum = mBoundingRadius + other->mBoundingRadius;
    collisionNormal = other->mTranslate - mTranslate;   
    float distancesq = collisionNormal.LengthSquared();
    // if distance squared > sum of radii squared, no collision!
    if ( distancesq > radiusSum*radiusSum )
    {
        return false;
    }
    
    // now do capsule collision
    return mWorldCapsule.ComputeCollision( other->mWorldCapsule, collisionNormal,
                                       collisionPoint, penetration );
}


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
    transform(0,3) = mTranslate.GetX();
    transform(1,3) = mTranslate.GetY();
    transform(2,3) = mTranslate.GetZ();

    IvSetWorldMatrix( transform );
    IvDrawCapsule( mLocalCapsule.GetSegment(), mLocalCapsule.GetRadius(), kOrange );
    
}
