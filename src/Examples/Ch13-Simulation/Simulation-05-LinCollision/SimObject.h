//===============================================================================
// @ SimObject.h
// ------------------------------------------------------------------------------
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
//===============================================================================

#ifndef __SimObjectDefs__
#define __SimObjectDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvQuat.h>
#include <IvVector3.h>
#include <IvMatrix33.h>
#include <IvBoundingSphere.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class SimObject
{
public:
    SimObject();
    virtual ~SimObject();

    virtual void Update( float dt );
    void Render();
    void SetMass( float mass );
    void SetBounds( float radius );
    void SetTranslate( const IvVector3& position );

    void HandleCollision( SimObject* other );

protected:
    void        InitializeMoments();
    void        Integrate( float dt );
    IvVector3   CurrentForce( const IvVector3& position, const IvVector3& velocity,
                              const IvQuat& orientation, const IvVector3& angularVelocity );
    IvVector3   CurrentTorque( const IvVector3& position, const IvVector3& velocity,
                              const IvQuat& orientation, const IvVector3& angularVelocity );
    bool        Colliding( const SimObject* other, 
                    IvVector3& collisionNormal, IvVector3& collisionPoint, float& penetration ) const;

    IvQuat      mRotate;
    IvVector3   mTranslate;

    float       mElasticity;
    IvVector3   mVelocity;
    IvVector3   mConstantForce; // part of force which is constant over interval
    
    // angular physics
    IvMatrix33  mMomentsInverse;
    IvVector3   mAngularMomentum;
    IvVector3   mAngularVelocity;
    IvVector3   mConstantTorque;    // part of torque which is constant over interval

private:
    // put here so child classes have to use SetMass() and SetBounds()
    float       mMass;

    // bounding data
    IvBoundingSphere    mSphere;
};

#endif
