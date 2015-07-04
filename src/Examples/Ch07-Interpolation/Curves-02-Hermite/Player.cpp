//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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
    
    IvVector3* samplePositions = new IvVector3[4];
    samplePositions[0].Set( -6.0f, 3.0f, 0.0f );
    samplePositions[1].Set( 0.0f, 0.0f, 0.0f );
    samplePositions[2].Set( -3.0f, -3.0f, 0.0f );
    samplePositions[3].Set( 6.0f, 0.0f, 0.0f );

    IvVector3* sampleTangents = new IvVector3[4];
    sampleTangents[0].Set( 2.0f, -1.0f, 0.0f );
    sampleTangents[1].Set( 1.0f, -2.0f, 0.0f );
    sampleTangents[2].Set( 2.0f, 0.0f, 0.0f );
    sampleTangents[3].Set( 0.0f, 5.0f, 0.0f );

    float* sampleTimes = new float[4];
    sampleTimes[0] = 0.0f;
    sampleTimes[1] = 2.0f;
    sampleTimes[2] = 6.0f;
    sampleTimes[3] = 9.0f;

    mCurve.Initialize( samplePositions, sampleTangents+1, sampleTangents, 
                       sampleTimes, 4 );

    delete [] samplePositions;
    delete [] sampleTangents;
    delete [] sampleTimes;

    mTime = 0.0f;
    mRun = true;

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


