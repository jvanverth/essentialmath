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
    
    mSamplePositions = new IvVector3[4];
    mSamplePositions[0].Set( -6.0f, 3.0f, 0.0f );
    mSamplePositions[1].Set( 0.0f, 0.0f, 0.0f );
    mSamplePositions[2].Set( -3.0f, -3.0f, 0.0f );
    mSamplePositions[3].Set( 6.0f, 0.0f, 0.0f );

    mSampleTimes = new float[4];
    mSampleTimes[0] = 0.0f;
    mSampleTimes[1] = 2.0f;
    mSampleTimes[2] = 6.0f;
    mSampleTimes[3] = 9.0f;

    mIn.Set( 2.0f, -1.0f, 0.0f );
    mOut.Set( 3.0f, 1.0f, 0.0f );
    mCurve.InitializeClamped( mSamplePositions, mSampleTimes, 4, mIn, mOut );
    SetWindowTitle("Clamped");

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
    delete [] mSamplePositions;
    delete [] mSampleTimes;

}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
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
    }

    if (IvGame::mGame->mEventHandler->IsKeyPressed('m'))
    {
        mMode = (mMode+1)%2;
        mCurve.Clean();
        switch (mMode)
        {
        case 0:
        default:
            mCurve.InitializeClamped( mSamplePositions, mSampleTimes, 4, mIn, mOut );
            SetWindowTitle("Clamped");
            break;

        case 1:
            mCurve.InitializeNatural( mSamplePositions, mSampleTimes, 4 );
            SetWindowTitle("Natural");
            break;
        }
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
    mCurve.Render();

    // build 4x4 matrix
    IvMatrix44 transform;
    transform.Translation( mTranslate );
    
    IvSetWorldMatrix( transform );

    IvDrawTeapot();

}


