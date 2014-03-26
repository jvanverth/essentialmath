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

#include <IvQuat.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    IvVector3   CurrentForce( const IvVector3& position, const IvVector3& velocity );

    IvVector3   mTranslate;         // current position

    float       mMass;              // object mass
    IvVector3   mVelocity;          // current linear velocity
    IvVector3   mConstantForce;     // part of force constant across time interval

    float       mSpringConstant;    // stiffness of spring    
};

#endif
