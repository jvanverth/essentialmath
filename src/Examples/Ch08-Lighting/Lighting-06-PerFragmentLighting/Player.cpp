//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvEventHandler.h>
#include <IvFragmentShader.h>
#include <IvImage.h>
#include <IvMath.h>
#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvResourceManager.h>
#include <IvShaderProgram.h>
#include <IvUniform.h>
#include <IvVector3.h>
#include <IvVertexShader.h>

#include <IvIndexBuffer.h>
#include <IvTexture.h>
#include <IvVertexBuffer.h>

#include "Player.h"
#include "Game.h"

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
    mRotate.Identity();
    mScale = 1.0f;
    mTranslate.Zero();

    mPlaneIndices = NULL;

    mPlaneVerts = NULL;

    mVertlitShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "vertlitShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "vertlitShader"));

    mFraglitShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "fraglitShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "fraglitShader"));

    mLightPos = IvVector4(0.0f, 0.0f, 1.0f, 0.0f);

    mShader = mFraglitShader;

    CreatePlane();
}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mPlaneIndices);

    IvRenderer::mRenderer->GetResourceManager()->Destroy(mPlaneVerts);
}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    // get scale, rotate, translate for this frame
    float s = 1.0f;
    float r = 0.0f;
    
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
    IvMatrix33 rotate;
    rotate.RotationZ(r);
    mRotate = rotate*mRotate;
    
    // set up translation
    IvVector3 xlate;
    xlate.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        xlate.SetX( xlate.GetX() - 3.0f*dt);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        xlate.SetX( xlate.GetX() + 3.0f*dt);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        xlate.SetY( xlate.GetY() - 3.0f*dt);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        xlate.SetY( xlate.GetY() + 3.0f*dt);
    }
    mTranslate += xlate;
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
        mScale = 1.0f;
        mTranslate.Zero();
    }

    r = 0.0f;
    static bool lightDirChanged = true;

    // set up rotate
    if (IvGame::mGame->mEventHandler->IsKeyDown('a'))
    {
        r -= kPI*0.0625f*dt;
        lightDirChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('d'))
    {
        r += kPI*0.0625f*dt;
        lightDirChanged = true;
    }

    if (lightDirChanged)
    {
        IvMatrix44 rotate;
        rotate.RotationX(r);
       
        mLightPos = rotate * mLightPos;
        mLightPos.Normalize();

        lightDirChanged = false;
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('f'))
    {
        mShader = mFraglitShader;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('v'))
    {
        mShader = mVertlitShader;
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
    transform(0,3) = mTranslate.GetX();
    transform(1,3) = mTranslate.GetY();
    transform(2,3) = mTranslate.GetZ();

    IvRenderer::mRenderer->SetShaderProgram(mShader);
    mShader->GetUniform("dirLightPosition")->SetValue(
        transform.AffineInverse() * mLightPos, 0);

    ::IvSetWorldMatrix(transform);

    // draw geometry
    DrawPlane();

}   // End of Player::Render()


//-------------------------------------------------------------------------------
// @ Player::DrawPlane()
//-------------------------------------------------------------------------------
// Draw vertex arrays for a plane centered around the origin
//-------------------------------------------------------------------------------
void
Player::DrawPlane()
{
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mPlaneVerts, mPlaneIndices);
} // End of Player::DrawPlane()
    
//-------------------------------------------------------------------------------
// @ Player::CreatePlane()
//-------------------------------------------------------------------------------
// Create vertex arrays for a plane centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreatePlane()                                    
{
    mPlaneVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kNPFormat, 4);

    IvNPVertex* tempVerts = (IvNPVertex*)(mPlaneVerts->BeginLoadData());

    tempVerts->position = IvVector3(-5.0f, -5.0f, 0.0f);
    tempVerts->normal = IvVector3(-1.0f, -1.0f, 0.15f);
    tempVerts->normal.Normalize();

    tempVerts++;

    tempVerts->position = IvVector3(5.0f, -5.0f, 0.0f);
    tempVerts->normal = IvVector3(1.0f, -1.0f, 0.15f);
    tempVerts->normal.Normalize();

    tempVerts++;

    tempVerts->position = IvVector3(-5.0f, 5.0f, 0.0f);
    tempVerts->normal = IvVector3(-1.0f, 1.0f, 0.15f);
    tempVerts->normal.Normalize();

    tempVerts++;

    tempVerts->position = IvVector3(5.0f, 5.0f, 0.0f);
    tempVerts->normal = IvVector3(1.0f, 1.0f, 0.15f);
    tempVerts->normal.Normalize();

    mPlaneVerts->EndLoadData();

    mPlaneIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(4);

    unsigned int* tempIndices = (unsigned int*)mPlaneIndices->BeginLoadData();

    unsigned int i;
    for (i = 0; i < 4; i++)
        tempIndices[i] = i;


    mPlaneIndices->EndLoadData();

}   // End of Player::CreatePlane()



