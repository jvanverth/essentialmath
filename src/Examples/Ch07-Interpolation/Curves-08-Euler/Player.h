//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player object
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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

class IvVertexBuffer;

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
    // current rotation matrix
    IvMatrix33  mRotate;

    // animation parameters
    bool  mRun;
    float mTime;

    // Euler interpolation parameters
    float mStartHeading, mStartPitch, mStartRoll;
    float mEndHeading, mEndPitch, mEndRoll;

    // quaternion interpolation parameters
    IvQuat mStartRotate, mEndRotate, mSlerpRotate;
    bool        mUseQuat;

    // array of points for drawing path of interpolation
    IvVector3   mStoredPoints[2][1280];
    unsigned int     mNumPoints;
    unsigned int     mFrameCounter;

    IvVertexBuffer*  mStrip0;
    IvVertexBuffer*  mStrip1;
    
};

#endif
