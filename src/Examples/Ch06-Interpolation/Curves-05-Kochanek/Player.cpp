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
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
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
    mTension = 0.0f;
    mBias = 0.0f;
    mContinuity = 0.0f;

    mTranslate.Zero();
    
    mTime = 0.0f;
    mRun = true;

    RegenerateCurve();

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
    // set new curve values
    bool rebuildCurve = false;
    if (IvGame::mGame->mEventHandler->IsKeyDown('t'))
    {
        mTension += 0.1f*dt;
        rebuildCurve = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('r'))
    {
        mTension -= 0.1f*dt;
        rebuildCurve = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('c'))
    {
        mContinuity += 0.1f*dt;
        rebuildCurve = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('x'))
    {
        mContinuity -= 0.1f*dt;
        rebuildCurve = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('b'))
    {
        mBias += 0.1f*dt;
        rebuildCurve = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('v'))
    {
        mBias -= 0.1f*dt;
        rebuildCurve = true;
    }

    if (IvGame::mGame->mEventHandler->IsKeyPressed(' '))
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
        mTension = 0.0f;
        mContinuity = 0.0f;
        mBias = 0.0f;
        rebuildCurve = true;
    }

    if (rebuildCurve)
    {
        RegenerateCurve();
    }

    if (mRun)
    {
        mTime += dt;
        mTranslate = mCurve.Evaluate( mTime );
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
    // draw curve
    mCurve.Render();

    // build 4x4 matrix
    IvMatrix44 transform;
    transform.Translation( mTranslate );
    
    IvSetWorldMatrix( transform );

    // draw the teapot
    IvDrawTeapot();

}

//-------------------------------------------------------------------------------
// @ Player::RegenerateCurve()
//-------------------------------------------------------------------------------
// Rebuild the curve
//
// Note that the endpoint tangents are based on the position of the other endpoint,
// i.e. the curve is cyclical.
//-------------------------------------------------------------------------------
void
Player::RegenerateCurve()
{
    mCurve.Clean();

    IvVector3 samplePositions[4];
    samplePositions[0].Set( -6.0f, 3.0f, 0.0f );
    samplePositions[1].Set( 0.0f, 0.0f, 0.0f );
    samplePositions[2].Set( -3.0f, -3.0f, 0.0f );
    samplePositions[3].Set( 6.0f, 0.0f, 0.0f );

    IvVector3 inTangents[3];
    IvVector3 outTangents[3];
    for (unsigned int i = 0; i < 3; ++i)
    {
        inTangents[i] = 
            0.5f*(1.0f-mTension)*(1.0f-mContinuity)*(1.0f+mBias)*(samplePositions[i+1]-samplePositions[i]);
        // standard incoming tangent
        if ( i < 2 )
            inTangents[i] += 0.5f*(1.0f-mTension)*(1.0f+mContinuity)*(1.0f-mBias)*(samplePositions[i+2]-samplePositions[i+1]);
        // cyclical incoming tangent
        else
            inTangents[i] += 0.5f*(1.0f-mTension)*(1.0f+mContinuity)*(1.0f-mBias)*(samplePositions[0]-samplePositions[i+1]);
        outTangents[i] = 
            0.5f*(1.0f-mTension)*(1.0f-mContinuity)*(1.0f-mBias)*(samplePositions[i+1]-samplePositions[i]);
        // standard outgoing tangent
        if ( i > 0 )          
            outTangents[i] += 0.5f*(1.0f-mTension)*(1.0f+mContinuity)*(1.0f+mBias)*(samplePositions[i]-samplePositions[i-1]);
        // cyclical outgoing tangent
        else          
            outTangents[i] += 0.5f*(1.0f-mTension)*(1.0f+mContinuity)*(1.0f+mBias)*(samplePositions[i]-samplePositions[3]);
    }

    float sampleTimes[4];
    sampleTimes[0] = 0.0f;
    sampleTimes[1] = 2.0f;
    sampleTimes[2] = 6.0f;
    sampleTimes[3] = 9.0f;

    mCurve.Initialize( samplePositions, inTangents, outTangents, 
                       sampleTimes, 4 );
}