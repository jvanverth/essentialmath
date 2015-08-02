//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>
#include <IvMath.h>
#include <IvAssert.h>

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
    mTranslate.Zero();
    
    IvVector3* samplePositions = new IvVector3[4];
    samplePositions[0].Set( -6.0f, 3.0f, 0.0f );
    samplePositions[1].Set( 0.0f, 0.0f, 0.0f );
    samplePositions[2].Set( -3.0f, -3.0f, 0.0f );
    samplePositions[3].Set( 6.0f, 0.0f, 0.0f );

    IvVector3* controls = new IvVector3[6];
    controls[0].Set( -3.0f, 3.0f, 0.0f );
    controls[1].Set( -0.5f, 1.5f, 0.0f );
    controls[2].Set( 0.5f, -1.5f, 0.0f );
    controls[3].Set( -5.0f, -1.5f, 0.0f );
    controls[4].Set( 0.0f, -5.0f, 0.0f );
    controls[5].Set( 4.0f, -3.0f, 0.0f );

    float* sampleTimes = new float[4];
    sampleTimes[0] = 0.0f;
    sampleTimes[1] = 2.0f;
    sampleTimes[2] = 6.0f;
    sampleTimes[3] = 9.0f;

    mCurve.Initialize( samplePositions, controls, sampleTimes, 4 );

    delete [] samplePositions;
    delete [] sampleTimes;
    delete [] controls;

    mTime = 0.0f;
    mRun = true;
    mMode = 0;

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
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{   
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        if (mRun)
        {
            mTime = 0.0f;
            mRun = false;
        }
        else
        {
            mRun = true;
        }
        
        IvGame::mGame->mEventHandler->KeyUp(' ');
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('m'))
    {
        mMode = (mMode+1)%2;
        
        IvGame::mGame->mEventHandler->KeyUp('m');
    }

    if (mRun)
    {
        mTime += dt;
    }

    // if time is greater than max time, continue
    if (mTime > 9.0f)
        return;
    
    float totaldistance = mCurve.GetLength();
    float distance;
    if ( mMode == 0 )
        distance = DistanceSinusoid( mTime/9.0f, .2f, .8f )*totaldistance;
    else
        distance = DistanceParabolic( mTime/9.0f, .2f, .8f )*totaldistance;

    float t = mCurve.FindParameterByDistance( 0.0f, distance );
    mTranslate = mCurve.Evaluate( t );

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::DistanceSinusoid()
//-------------------------------------------------------------------------------
// Sinusoidal speed control
//-------------------------------------------------------------------------------
float 
Player::DistanceSinusoid( float t, float k1, float k2 )
{
    // make sure ranges are correct
    ASSERT( 0.0f <= t && t <= 1.0f );
    ASSERT( 0.0f <= k1 && k1 <= k2 && k1 <= 1.0f );
    ASSERT( 0.0f <= k2 && k2 <= 1.0f );

    const float kHalfPIrecip = 1.0f/kHalfPI;
    float f = k1*kHalfPIrecip + k2 - k1 + (1.0f - k2)*kHalfPIrecip;
    float result;
    if ( t < k1 )
    { 
        result = k1*kHalfPIrecip*(IvSin(t/k1*kHalfPI-kHalfPI) + 1.0f);
    }
    else if (t > k2 )
    {
        result = k1*kHalfPIrecip + k2 - k1 +
            (1.0f - k2)*kHalfPIrecip*IvSin(((t-k2)/(1.0f-k2))*kHalfPI);
    }
    else
    {
        result = k1*kHalfPIrecip + t - k1;
    }

    return result/f;

}   // End of Player::DistanceSinusoid()


//-------------------------------------------------------------------------------
// @ Player::DistanceParabolic()
//-------------------------------------------------------------------------------
// Parabolic speed control
//-------------------------------------------------------------------------------
float 
Player::DistanceParabolic( float t, float k1, float k2 )
{
    // make sure ranges are correct
    ASSERT( 0.0f <= t && t <= 1.0f );
    ASSERT( 0.0f <= k1 && k1 <= k2 && k1 <= 1.0f );
    ASSERT( 0.0f <= k2 && k2 <= 1.0f );

    // calculate maximum velocity
    float v0 = 2.0f/(k2 - k1 + 1.0f);

    float result = 0.0f;
    if ( t < k1 )
    { 
        result = 0.5f*v0*t*t/k1;
    }
    else if (t > k2 )
    {
        result = 0.5f*v0*k1 + v0*(k2-k1) + (v0 - 0.5f*(v0*(t-k2)/(1.0f-k2)))*(t - k2);
    }
    else
    {
        result = 0.5f*v0*k1 + v0*(t-k1);
    }

    return result;

}   // End of Player::DistanceParabolic()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    mCurve.Render();

    // build 4x4 matrix
    IvMatrix44 transform;
    transform.Translation( mTranslate );
    
    IvSetWorldMatrix( transform );

    IvDrawTeapot();

}


