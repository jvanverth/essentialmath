//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// In this demo, we show various methods for generating random distributions: two
// on a sphere and two on a disc.  In the first sphere and disc methods we try 
// a slightly incorrect approach that leads to clumping in the "corners" in the 
// sphere case and in the center in the disc case.  In the second set we correct
// those.  In the sphere case the correction is to do rejection sampling to remove 
// points outside of the sphere.  In the disc case the correction is to take the 
// square root of the random radius before using it.
//
// The clustering in the "incorrect" sphere is a little difficult to see. If you 
// rotate the sphere, you should notice that it clumps longitudinally every quarter 
// sphere.  Note also that the disk looks as if it's a dome.  This is an optical 
// illusion due to the viewing angle.
//
// The key commands are:
//
// U, O - rotate around z axis
// P, : - uniformly scale
// M    - switch generation method for random points
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvMatrix44.h>
#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvResourceManager.h>
#include <IvVertexBuffer.h>
#include <IvXorshift.h>

#include "Game.h"
#include "Player.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static IvXorshift rng(12021966);

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
    mRotate.Identity();
    mScale = 1.0f;
    
    

	mVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 1500,
                                                                                NULL, kDefaultUsage);
    IvCPVertex* dataPtr = (IvCPVertex*) mVertices->BeginLoadData();
    for (unsigned int i = 0; i < 1500; ++i)
    {
        dataPtr[i].color = kOrange;
        dataPtr[i].position = GenSpherePointIncorrect();
    }
    mVertices->EndLoadData();
    
    mMode = 0;

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    if ( mVertices )
        IvRenderer::mRenderer->GetResourceManager()->Destroy( mVertices );
}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::GenSpherePointIncorrect()
//-------------------------------------------------------------------------------
// Generate a point on a sphere, but incorrectly
//-------------------------------------------------------------------------------
IvVector3 
Player::GenSpherePointIncorrect()
{
    IvVector3 randomPos(2.0f*rng.RandomFloat() - 1.0f,
                        2.0f*rng.RandomFloat() - 1.0f,
                        2.0f*rng.RandomFloat() - 1.0f);
    randomPos.Normalize();
    randomPos *= 5.0f;

    return randomPos;
}

//-------------------------------------------------------------------------------
// @ Player::GenSpherePointCorrect()
//-------------------------------------------------------------------------------
// Generate a point on a sphere correctly
//-------------------------------------------------------------------------------
IvVector3 
Player::GenSpherePointCorrect()
{
    for (;;)
    {
        IvVector3 randomPos(2.0f*rng.RandomFloat() - 1.0f,
                            2.0f*rng.RandomFloat() - 1.0f,
                            2.0f*rng.RandomFloat() - 1.0f);
        if ( randomPos.Length() > 1.0f )
        {
            continue;
        }
        randomPos.Normalize();
        randomPos *= 5.0f;

        return randomPos;
    }
}

//-------------------------------------------------------------------------------
// @ Player::GenSpherePointIncorrect()
//-------------------------------------------------------------------------------
// Generate a point on a cisc, but incorrectly
//-------------------------------------------------------------------------------
IvVector3 
Player::GenDiscPointIncorrect()
{
    float r = rng.RandomFloat();
    float theta = kTwoPI*rng.RandomFloat();
    float sintheta, costheta;
    IvSinCos(theta, sintheta, costheta);

    IvVector3 randomPos(5.0f*r*costheta, 5.0f*r*sintheta, 0.0f);
    return randomPos;
}

//-------------------------------------------------------------------------------
// @ Player::GenDiscPointCorrect()
//-------------------------------------------------------------------------------
// Generate a point on a disc correctly
//-------------------------------------------------------------------------------
IvVector3 
Player::GenDiscPointCorrect()
{
    float r = IvSqrt(rng.RandomFloat());
    float theta = kTwoPI*rng.RandomFloat();
    float sintheta, costheta;
    IvSinCos(theta, sintheta, costheta);

    IvVector3 randomPos(5.0f*r*costheta, 5.0f*r*sintheta, 0.0f);
    return randomPos;
}


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    float s = 1.0f;
    float r = 0.0f;
    IvQuat rotate;
    IvVector3 xlate;
    IvMatrix44 transform;
    
    // set up scaling
    if (IvGame::mGame->mEventHandler->IsKeyDown(';'))
    {
        s -= 0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('p'))
    {
        s += 0.25f*dt;
    }
    mScale *= s;
    
    // set up rotate
    if (IvGame::mGame->mEventHandler->IsKeyDown('o'))
    {
        r -= kPI*0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('u'))
    {
        r += kPI*0.25f*dt;
    }
    rotate.Set(IvVector3::zAxis, r);
    mRotate = rotate*mRotate;
	mRotate.Normalize();
        
    if (IvGame::mGame->mEventHandler->IsKeyDown('m'))
    {
        IvGame::mGame->mEventHandler->KeyUp('m');
        mMode = (mMode+1)%4;
        IvCPVertex* dataPtr = (IvCPVertex*) mVertices->BeginLoadData();
        for (unsigned int i = 0; i < 1500; ++i)
        {
            switch (mMode)
            {
            default:
            case 0:
                dataPtr[i].position = GenSpherePointIncorrect();
                break;
            case 1:
                dataPtr[i].position = GenSpherePointCorrect();
                break;
            case 2:
                dataPtr[i].position = GenDiscPointIncorrect();
                break;
            case 3:
                dataPtr[i].position = GenDiscPointCorrect();
                break;
            }
        }
        mVertices->EndLoadData();
    }

    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
        mScale = 1.0f;
        mTranslate.Zero();
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
    transform(0,0) *= mScale;
    transform(1,0) *= mScale;
    transform(2,0) *= mScale;
    transform(0,1) *= mScale;
    transform(1,1) *= mScale;
    transform(2,1) *= mScale;
    transform(0,2) *= mScale;
    transform(1,2) *= mScale;
    transform(2,2) *= mScale;
    
    // set current local-to-world matrix
    IvSetWorldMatrix( transform );

   // draw geometry
    IvRenderer::mRenderer->Draw( kPointListPrim, mVertices );
}


