//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
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

class IvVertexBuffer;

enum Mode { kEuler = 0, kMidpoint, kRK4, kVerlet, kNaiveImplicit, kPredictorCorrector, 
   kSymplectic, kMaxMode };

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

    Mode        mMode;              // current integration method

    unsigned int     mFrameCounter;
    IvVertexBuffer*  mOrbit;
    IvVertexBuffer*  mPath;
};

#endif
