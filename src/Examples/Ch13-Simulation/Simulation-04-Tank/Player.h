//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __PlayerDefs__
#define __PlayerDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix33.h>
#include <IvQuat.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvSpatial;

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    void        Integrate( float dt );
    IvVector3   CurrentForce( const IvVector3& position, const IvVector3& velocity,
                              const IvQuat& orientation, const IvVector3& angularVelocity );
    IvVector3   CurrentTorque( const IvVector3& position, const IvVector3& velocity,
                              const IvQuat& orientation, const IvVector3& angularVelocity );

    float       mScale;         // current scale
    IvQuat      mRotate;        // current orientation
    IvVector3   mTranslate;     // current position
    IvSpatial* mTank;
    IvSpatial* mTurret;
    IvSpatial* mBarrel;

    float      mMass;          // mass of object

    // linear dynamics
    IvVector3   mVelocity;      // current velocity
    IvVector3   mForce1, mForce2;   // drive forces
    IvVector3   mConstantForce;     // part of force which is constant over interval
    
    // rotational dynamics
    IvMatrix33  mMomentsInverse;    // inverse of inertial tensor
    IvVector3   mAngularMomentum;   // current angular momentum
    IvVector3   mAngularVelocity;   // current angular velocity
    IvVector3   mConstantTorque;    // part of torque which is constant over interval (?)
};

#endif
