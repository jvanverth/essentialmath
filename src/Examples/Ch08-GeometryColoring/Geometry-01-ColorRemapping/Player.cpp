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

    unsigned int i;
    for (i = 0; i < NUM_TEX; i++)
        mTextures[i] = nullptr;

    mCurrentTexIndex = 0;

    // Load the soup can image into this texture
    IvImage* image = IvImage::CreateFromFile("image.tga");
    if (image)
    {
        mTextures[0] = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(),
            image->GetPixels(), kImmutableUsage);

        delete image;
        image = 0;
    }

    mTextures[0]->SetMagFiltering(kBilerpTexMagFilter);
    mTextures[0]->SetMinFiltering(kBilerpTexMinFilter);
    mTextures[0]->SetAddressingU(kWrapTexAddr);
    mTextures[0]->SetAddressingV(kWrapTexAddr);


    const unsigned int size = 256;
    mTextures[1] = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
        kRGB24TexFmt, size, size, nullptr, kDefaultUsage);
    mTextures[1]->SetMagFiltering(kBilerpTexMagFilter);
    mTextures[1]->SetMinFiltering(kBilerpTexMinFilter);
    mTextures[1]->SetAddressingU(kWrapTexAddr);
    mTextures[1]->SetAddressingV(kWrapTexAddr);

    unsigned char* pixels = (unsigned char*)(mTextures[1]->BeginLoadData(0));

    unsigned char* tempPixels = pixels;

    unsigned int j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            tempPixels[0] = i;
            tempPixels[1] = j;
            tempPixels[2] = 128;
            
            tempPixels += 3;
        }
    }

    mTextures[1]->EndLoadData(0);

    mShaderClamp = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShaderClamp"));

    mShaderScale = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShaderScale"));

    mShaderToneMap = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShaderToneMap"));
    
    mShaderClamp->GetUniform("Texture")->SetValue(mTextures[0]);
    mShaderScale->GetUniform("Texture")->SetValue(mTextures[0]);
    mShaderToneMap->GetUniform("Texture")->SetValue(mTextures[0]);

    mColorScale = 1.0f;

    mShaderClamp->GetUniform("scale")->SetValue(1.0f, 0);
    mShaderScale->GetUniform("scale")->SetValue(1.0f, 0);
    mShaderToneMap->GetUniform("scale")->SetValue(1.0f, 0);

    IvRenderer::mRenderer->SetShaderProgram(mShaderClamp);

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
    
    unsigned int i;
    for (i = 0; i < NUM_TEX; i++)
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mTextures[i]);
    }

    IvRenderer::mRenderer->GetResourceManager()->Destroy(mShaderScale);
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mShaderClamp);
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mShaderToneMap);

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

    if (IvGame::mGame->mEventHandler->IsKeyDown('-'))
    {
        mColorScale -= 0.01f;
        if (mColorScale < 0.01f)
        {
            mColorScale = 0.01f;
        }

        mShaderClamp->GetUniform("scale")->SetValue(mColorScale, 0);
        mShaderScale->GetUniform("scale")->SetValue(mColorScale, 0);
        mShaderToneMap->GetUniform("scale")->SetValue(mColorScale, 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('='))
    {
        mColorScale += 0.01f;
        if (mColorScale > 16.0f)
        {
            mColorScale = 16.0f;
        }

        mShaderClamp->GetUniform("scale")->SetValue(mColorScale, 0);
        mShaderScale->GetUniform("scale")->SetValue(mColorScale, 0);
        mShaderToneMap->GetUniform("scale")->SetValue(mColorScale, 0);
    }

    // change texture
    if (IvGame::mGame->mEventHandler->IsKeyPressed('t'))
    {
        mCurrentTexIndex = (mCurrentTexIndex + 1) % NUM_TEX;

        mShaderScale->GetUniform("Texture")->SetValue(mTextures[mCurrentTexIndex]);
        mShaderClamp->GetUniform("Texture")->SetValue(mTextures[mCurrentTexIndex]);
    }

    if (IvGame::mGame->mEventHandler->IsKeyPressed('s'))
    {
        if (IvRenderer::mRenderer->GetShaderProgram() == mShaderClamp)
            IvRenderer::mRenderer->SetShaderProgram(mShaderScale);
        else if (IvRenderer::mRenderer->GetShaderProgram() == mShaderScale)
            IvRenderer::mRenderer->SetShaderProgram(mShaderToneMap);
        else
            IvRenderer::mRenderer->SetShaderProgram(mShaderClamp);
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
        kTCPFormat, 4, nullptr, kDefaultUsage);

    IvTCPVertex* tempVerts = (IvTCPVertex*)(mPlaneVerts->BeginLoadData());

    tempVerts->position = IvVector3(-5.0f, -5.0f, 0.0f);
    tempVerts->texturecoord = IvVector2(1.0f, 0.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    tempVerts++;

    tempVerts->position = IvVector3(5.0f, -5.0f, 0.0f);
    tempVerts->texturecoord = IvVector2(1.0f, 1.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    tempVerts++;

    tempVerts->position = IvVector3(-5.0f, 5.0f, 0.0f);
    tempVerts->texturecoord = IvVector2(0.0f, 0.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    tempVerts++;

    tempVerts->position = IvVector3(5.0f, 5.0f, 0.0f);
    tempVerts->texturecoord = IvVector2(0.0f, 1.0f);
    tempVerts->color.mAlpha = 
    tempVerts->color.mRed = 
    tempVerts->color.mGreen = 
    tempVerts->color.mBlue = 255; 

    mPlaneVerts->EndLoadData();

    mPlaneIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(4, nullptr,
                                                                                   kDefaultUsage);

    UInt32* tempIndices = (UInt32*)mPlaneIndices->BeginLoadData();

    UInt32 i;
    for (i = 0; i < 4; i++)
        tempIndices[i] = i;


    mPlaneIndices->EndLoadData();

}   // End of Player::CreatePlane()



