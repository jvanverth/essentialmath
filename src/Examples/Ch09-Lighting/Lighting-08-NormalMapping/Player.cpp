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

    mPlaneIndices = nullptr;

    mPlaneVerts = nullptr;

    mColorTexture = nullptr;
    mNormalTexture = nullptr;

    // Load the color and normal textures
    IvImage* image = IvImage::CreateFromFile("brickwork-texture.tga");
    if (image)
    {
        mColorTexture = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(), image->GetPixels(), kImmutableUsage);

        delete image;
        image = 0;
    }
    mColorTexture->SetMagFiltering(kBilerpTexMagFilter);
    mColorTexture->SetMinFiltering(kBilerpTexMinFilter);
    mColorTexture->SetAddressingU(kWrapTexAddr);
    mColorTexture->SetAddressingV(kWrapTexAddr);

    image = IvImage::CreateFromFile("brickwork_normal-map.tga");
    if (image)
    {
        mNormalTexture = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(), image->GetPixels(), kImmutableUsage);

        delete image;
        image = 0;
    }
    mNormalTexture->SetMagFiltering(kBilerpTexMagFilter);
    mNormalTexture->SetMinFiltering(kBilerpTexMinFilter);
    mNormalTexture->SetAddressingU(kWrapTexAddr);
    mNormalTexture->SetAddressingV(kWrapTexAddr);

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "normalShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "normalShader"));

    mShader->GetUniform("NormalTexture")->SetValue(mNormalTexture);
    mShader->GetUniform("ColorTexture")->SetValue(mColorTexture);

    mLightPos = IvVector3(-0.8f, 0.8f, 0.8f);
    mLightPos.Normalize();

    IvRenderer::mRenderer->SetShaderProgram(mShader);

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
    
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mNormalTexture);
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mColorTexture);

    IvRenderer::mRenderer->GetResourceManager()->Destroy(mShader);
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
        xlate.x -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        xlate.x += 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        xlate.y -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        xlate.y += 3.0f*dt;
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
        IvMatrix33 rotate;
        rotate.RotationY(r);
       
        mLightPos = rotate * mLightPos;
        mLightPos.Normalize();

        lightDirChanged = false;
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
    transform(0,3) = mTranslate.x;
    transform(1,3) = mTranslate.y;
    transform(2,3) = mTranslate.z;

    mShader->GetUniform("dirLightDirection")->SetValue(mLightPos, 0);

    IvSetWorldMatrix(transform);

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
        kTCPFormat, 4, nullptr, kDefaultUsage);

    IvTCPVertex* tempVerts = (IvTCPVertex*)(mPlaneVerts->BeginLoadData());

    tempVerts->position = IvVector3(0.0f, -5.0f, -5.0f);
    tempVerts->texturecoord = IvVector2(1.0f, 0.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    tempVerts++;

    tempVerts->position = IvVector3(0.0f, -5.0f, 5.0f);
    tempVerts->texturecoord = IvVector2(1.0f, 1.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    tempVerts++;

    tempVerts->position = IvVector3(0.0f, 5.0f, -5.0f);
    tempVerts->texturecoord = IvVector2(0.0f, 0.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    tempVerts++;

    tempVerts->position = IvVector3(0.0f, 5.0f, 5.0f);
    tempVerts->texturecoord = IvVector2(0.0f, 1.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    mPlaneVerts->EndLoadData();

    mPlaneIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(4, nullptr, kDefaultUsage);

    unsigned int* tempIndices = (unsigned int*)mPlaneIndices->BeginLoadData();

    unsigned int i;
    for (i = 0; i < 4; i++)
        tempIndices[i] = i;

    mPlaneIndices->EndLoadData();

}   // End of Player::CreatePlane()



