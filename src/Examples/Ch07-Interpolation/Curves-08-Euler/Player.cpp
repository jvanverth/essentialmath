//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This code shows the problems with linearly interpolating Euler angles.  The 
// yellow lines trace two points on the cube, showing how one doesn't rotate 
// directly and the other moves out of alignment and then back to the same 
// location.  You can toggle between using Euler angles and quaternions.
//
// Controls
// --------
// ESC: quit
// SPACE: reset/stop animation
// X: toggle between Euler angles and quaternion
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>
#include <IvResourceManager.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>

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
    // set animation params
    mRun = false;
    mTime = 0.0f;

    // set euler params
    mStartHeading = 0.0f;
    mStartPitch = 0.0f;
    mStartRoll = 0.0f;
    mEndHeading = kPI;
    mEndPitch = kPI * 0.5f;
    mEndRoll= 0.0f;

    // set quaternion params
    mStartRotate.Identity();
    mEndRotate = IvQuat(IvVector3::yAxis, kPI * 0.5f ) 
        * IvQuat( IvVector3::zAxis, kPI );
    mSlerpRotate.Identity();
    mUseQuat = false;

    // set object transform
    mRotate.Identity();

    mStrip0 = 0;
    mStrip1 = 0;
    mNumPoints = 0;
    mFrameCounter = 0;

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
    IvMatrix33 rotateX, rotateY, rotateZ;
    IvMatrix33 rotate;
    float heading = 0.0f, pitch = 0.0f, roll = 0.0f;

    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        if (mRun)
        {
            // reset animations
            mTime = 0.0f;
            mRun = false;
            heading = mStartHeading;
            pitch = mStartPitch;
            roll = mStartRoll;
            mSlerpRotate = mStartRotate;
            mNumPoints = 0;
            mRotate.Rotation( heading, pitch, roll );
        }
        else
        {
            // start 'er up again
            mRun = true;
        }

        IvGame::mGame->mEventHandler->KeyUp(' ');
    }

    // swap orientation reps
    if (IvGame::mGame->mEventHandler->IsKeyDown('x'))
    {
        mUseQuat = !mUseQuat;

        IvGame::mGame->mEventHandler->KeyUp('x');
        mNumPoints = 0;
    }


    if (mRun)
    {
        // get the next time step
        mTime += dt;
        if ( mTime > 8.0f )
            mTime = 8.0f;
        float alpha = mTime/8.0f;

        // interpolate
        heading = mStartHeading*(1.0f - alpha) + mEndHeading*alpha;
        pitch = mStartPitch*(1.0f - alpha) + mEndPitch*alpha;
        roll = mStartRoll*(1.0f - alpha) + mEndRoll*alpha;
        Slerp(mSlerpRotate, mStartRotate, mEndRotate, alpha);

        // set rotation matrix based on current orientation rep
        if (mUseQuat)
        {
            mRotate.Rotation(mSlerpRotate);
        }
        else
        {
            mRotate.Rotation( heading, pitch, roll );
        }

        // add new path points
        if (mNumPoints < 1280 && mFrameCounter == 0)
        {
            mStoredPoints[0][mNumPoints] = mRotate*IvVector3(1.0f, -1.0f, 1.0f);
            mStoredPoints[1][mNumPoints++] = mRotate*IvVector3(-1.0f, 0.0f, -1.0f);
        }
        mFrameCounter = (mFrameCounter + 1) % 10;
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
    IvMatrix44 transform(mRotate);
    IvSetWorldMatrix(transform);
    
    // draw cube
    IvDrawCube( kWhite );
                    
    // draw paths
    if ( mNumPoints < 2 )
        return;

    IvSetWorldIdentity();

    IvDepthTestFunc oldDepthTest = IvRenderer::mRenderer->GetDepthTest();
    IvRenderer::mRenderer->SetDepthTest(kDisableDepthTest);              // Disable Depth Testing

    if ( !mStrip0 )
    {
        mStrip0 = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 1280,
                                                                                  NULL, kDefaultUsage);
    }

    IvCPVertex* curveDataPtr = (IvCPVertex*) mStrip0->BeginLoadData();
    for (unsigned int i = 0; i < mNumPoints; ++i)
    {
        curveDataPtr[i].position = mStoredPoints[0][i];
        curveDataPtr[i].color = kOrange;
    }
    (void) mStrip0->EndLoadData();
    IvRenderer::mRenderer->Draw( kLineStripPrim, mStrip0, mNumPoints );

    if ( !mStrip1 )
    {
        mStrip1 = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 1280,
                                                                                  NULL, kDefaultUsage);
    }

    curveDataPtr = (IvCPVertex*) mStrip1->BeginLoadData();
    for (unsigned int i = 0; i < mNumPoints; ++i)
    {
        curveDataPtr[i].position = mStoredPoints[1][i];
        curveDataPtr[i].color = kOrange;
    }
    (void) mStrip1->EndLoadData();
    IvRenderer::mRenderer->Draw( kLineStripPrim, mStrip1, mNumPoints );

    IvRenderer::mRenderer->SetDepthTest(oldDepthTest);
}

