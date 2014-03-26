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
    // run parameters
    float       mTime;
    bool        mRun;
    bool        mCorrection;

    // current lerped and slerped orientations
    IvQuat      mLerpRotate;
    IvQuat      mSlerpRotate;

    // start and end orientations to be interpolated
    IvQuat      mStartRotate;
    IvQuat      mEndRotate;
};

#endif
