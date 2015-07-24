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
#include <IvMatrix33.h>
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
    IvVector3* samplePositions = new IvVector3[5];
    samplePositions[0].Set( -6.0f, 3.0f, -5.0f );
    samplePositions[1].Set( 0.0f, 0.0f, 0.0f );
    samplePositions[2].Set( -3.0f, -1.0f, 5.0f );
    samplePositions[3].Set( 6.0f, 1.0f, 2.0f );
    samplePositions[4].Set( 12.0f, -3.0, -1.0f );

    float* sampleTimes = new float[5];
    sampleTimes[0] = 0.0f;
    sampleTimes[1] = 3.0f;
    sampleTimes[2] = 6.0f;
    sampleTimes[3] = 9.0f;
    sampleTimes[4] = 12.0f;

    mCurve.InitializeNatural( samplePositions, sampleTimes, 5 );

    delete [] samplePositions;
    delete [] sampleTimes;

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
    // update based on input
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
        mMode = (mMode + 1)%4;
        
        IvGame::mGame->mEventHandler->KeyUp('m');
    }

    if (mRun)
    {
        mTime += dt;
        // stop just before end so we don't end up with zero vector
        // when we look ahead
        if ( mTime > 11.5f )
            mTime = 11.5f;
    }

    // now we set up the camera
    // get eye position
    IvVector3 eye = mCurve.Evaluate( mTime );
    IvVector3 viewDir;
    IvVector3 viewSide;
    IvVector3 viewUp;

    // set parameters depending on what mode we're in

    // Frenet frame
    if ( mMode == 3 )
    {   
        IvVector3 T = mCurve.Velocity( mTime );
        IvVector3 a = mCurve.Acceleration( mTime );
        IvVector3 B = T.Cross( a );
        B.Normalize();
        T.Normalize();
        IvVector3 N = B.Cross( T );
    
        viewDir = T;
        viewSide = -N;     // have to negate to get from RH frame to LH frame
        viewUp = B;        
    }
    else
    {
        IvVector3 lookAt;
        
        // look same direction all the time
        if ( mMode == 2 ) 
        {
            viewDir = IvVector3::xAxis;
        }
        // look along tangent
        else if ( mMode == 1 )
        {
            viewDir = mCurve.Velocity( mTime );
        }
        // look ahead .5 in parameter
        else if ( mMode == 0 )
        {
            viewDir = mCurve.Evaluate( mTime+0.5f ) - eye;
        }

        // compute view vectors
        viewDir.Normalize();
        viewUp = IvVector3::zAxis - IvVector3::zAxis.Dot(viewDir)*viewDir;
        viewUp.Normalize();
        viewSide = viewDir.Cross(viewUp);
    }

    // now set up matrices
    // build transposed rotation matrix
    IvMatrix33 rotate;
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        rotate.SetRows( viewSide, viewUp, -viewDir );
    }
    else
    {
        rotate.SetRows( viewSide, viewUp, viewDir );
    }
    // transform translation
    IvVector3 eyeInv = -(rotate*eye);
    // build 4x4 matrix
    IvMatrix44 matrix(rotate);
    matrix(0,3) = eyeInv.x;
    matrix(1,3) = eyeInv.y;
    matrix(2,3) = eyeInv.z;

    ::IvSetViewMatrix( matrix );

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render the curve
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    IvMatrix44 mat;
    ::IvSetWorldMatrix( mat );

    mCurve.Render();
}


