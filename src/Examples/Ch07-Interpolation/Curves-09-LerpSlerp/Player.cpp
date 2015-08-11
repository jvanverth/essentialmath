//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This code shows the different between linear interpolation and spherical linear 
// interpolation of quaternions.  The green cube uses slerp, the red cube uses lerp.  
// The key thing to notice is that the two cubes are not coincident -- the green cube 
// outraces the red cube until the middle, when the red cube races ahead.  
// You can activate Jonathan Blow's spline-based correction for linear interpolation 
// by hitting C, and the linear interpolation speed matches the slerp interpolation 
// much better.
// 
// Controls
// --------
// ESC: quit
// SPACE: reset/stop animation
// C: toggle Blow's correction on/off
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>

#include "Game.h"
#include "Player.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

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
    // run parameters
    mCorrection = false;
    mRun = false;
    mTime = 0.0f;

    // our current lerped and slerped orientations
    mLerpRotate.Identity();
    mSlerpRotate.Identity();

    // our start and end orientations
    mStartRotate.Identity();
    mEndRotate = IvQuat(IvVector3::yAxis, kPI * 0.5f)
        * IvQuat(IvVector3::zAxis, kPI);

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
// @ correction()
//-------------------------------------------------------------------------------
// Try to correct interpolation parameter for lerp to approximate speed of slerp.
//-------------------------------------------------------------------------------
float correction( float t, float cosAlpha )
{
    float factor = 1.0f - 0.7878088f*cosAlpha;
    float k = 0.5069269f;
    factor *= factor;
    k *= factor;

    float b = 2*k;
    float c = -3*k;
    float d = 1 + k;

    return t*(b*t + c) + d;
}


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyPressed(' '))
    {
        if (mRun)
        {
            mTime = 0.0f;
            mRun = false;
            mLerpRotate = mStartRotate;
            mSlerpRotate = mStartRotate;
        }
        else
        {
            mRun = true;
        }
    }

    // toggle spline correction
    if (IvGame::mGame->mEventHandler->IsKeyPressed('c'))
    {
        mCorrection = !mCorrection;
    }

    // animate!
    if (mRun)
    {
        if ( dt > 0.25f )
            dt = 0.25f;
        mTime += dt;
        if ( mTime > 10.0f )
            mTime = 10.0f;

        float alpha = mTime/10.0f;

        Slerp(mSlerpRotate, mStartRotate, mEndRotate, alpha);
        if (mCorrection)
            alpha *= correction(alpha, Dot(mStartRotate, mEndRotate) );
        Lerp(mLerpRotate, mStartRotate, mEndRotate, alpha );
    }

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    // build 4x4 matrix
    mLerpRotate.Normalize();
    IvMatrix44 transform( mLerpRotate );

    // draw lerp cube
    transform *= 1.5f;
    transform(0,3) = 0.0f;
    transform(1,3) = 0.0f;
    transform(2,3) = 0.0f;
    transform(3,3) = 1.0f;
    
    IvSetWorldMatrix( transform );    
    IvDrawCube( kRed );
                    
    // draw slerp cube
    transform.Identity();
    (void) transform.Rotation( mSlerpRotate );
    transform *= 1.5f;
    transform(0,3) = 0.0f;
    transform(1,3) = 0.0f;
    transform(2,3) = 0.0f;
    transform(3,3) = 1.0f;
    
    IvSetWorldMatrix( transform );    
    IvDrawCube( kGreen );
}


