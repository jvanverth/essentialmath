//===============================================================================
// @ CollisionObject.cpp
// ------------------------------------------------------------------------------
// Class for managing a simple object that can collide with other objects
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This class has three main members: its position, a velocity, and a bounding
// sphere.  The velocity is used to update the position, and to keep it contained
// within a [-1,1] box.  The bounding sphere is for testing collision, and is
// updated to be centered around the current position.
//
// When rendering, we use yellow for the normal case, and red if a collision has
// been detected.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix44.h>
#include <IvRendererHelp.h>
#include <IvXorshift.h>

#include "CollisionObject.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static IvXorshift rng(12021966);

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ CollisionObject::CollisionObject()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
CollisionObject::CollisionObject()
{
    // set random position within [-1,1] box
    // reduced to make room for bounding sphere radius
    mPosition.Set( 1.6f*rng.RandomFloat() - 0.8f,
                   1.6f*rng.RandomFloat() - 0.8f,
                   1.6f*rng.RandomFloat() - 0.8f );

    // set random velocity direction
    float r;
    do 
    { 
        mVelocity.Set( 2.0f*rng.RandomFloat()-1.0f,
                       2.0f*rng.RandomFloat()-1.0f,
                       2.0f*rng.RandomFloat()-1.0f ); 
        r = mVelocity.LengthSquared();
    } while ( r > 1.0f || r == 0.0f );

    mVelocity.Normalize();
    mVelocity *= 0.75f;

    // set bounding sphere 
    mSphere.SetCenter( mPosition );
    mSphere.SetRadius( 0.2f );

}   // End of CollisionObject::CollisionObject()


//-------------------------------------------------------------------------------
// @ CollisionObject::~CollisionObject()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
CollisionObject::~CollisionObject()
{
}   // End of CollisionObject::~CollisionObject()


//-------------------------------------------------------------------------------
// @ CollisionObject::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
CollisionObject::Update( float dt )
{
    // update position
    mPosition += mVelocity*dt;

    // adjust velocity to keep us inside box
    float radius = mSphere.GetRadius();
    if ( mPosition.x+radius > 1.0f && mVelocity.x > 0.0f )
        mVelocity.x = -mVelocity.x;
    if ( mPosition.x-radius < -1.0f && mVelocity.x < 0.0f )
        mVelocity.x = -mVelocity.x;

    if ( mPosition.y+radius > 1.0f && mVelocity.y > 0.0f )
        mVelocity.y = -mVelocity.y;
    if ( mPosition.y-radius < -1.0f && mVelocity.y < 0.0f )
        mVelocity.y = -mVelocity.y;

    if ( mPosition.z+radius > 1.0f && mVelocity.z > 0.0f )
        mVelocity.z = -mVelocity.z;
    if ( mPosition.z-radius < -1.0f && mVelocity.z < 0.0f )
        mVelocity.z = -mVelocity.z;

    // update sphere
    mSphere.SetCenter( mPosition );

    // make sure we're tagged as not currently colliding
    // will be updated by ObjectDB
    mColliding = false;

}   // End of CollisionObject::Update()


//-------------------------------------------------------------------------------
// @ CollisionObject::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
CollisionObject::Render()                                   
{   
    // translate sphere to world position
    IvMatrix44 xform;
    xform.Translation( mPosition );

    ::IvSetWorldMatrix( xform );
    // render in red if colliding, yellow otherwise
    if (mColliding)
        IvDrawSphere( mSphere.GetRadius(), kRed );
    else
        IvDrawSphere( mSphere.GetRadius(), kYellow );

}   // End of CollisionObject::Render()


//-------------------------------------------------------------------------------
// @ CollisionObject::HasCollision()
//-------------------------------------------------------------------------------
// Our simple collision test.
// Returns true if the bounding spheres of the two objects are colliding
//-------------------------------------------------------------------------------
bool 
CollisionObject::HasCollision( const CollisionObject* other ) const
{    
    return mSphere.Intersect( other->mSphere );
}
