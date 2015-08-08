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

    mRadius = 3.0f;

    mCylinderIndices = NULL;

    int i;
    for (i = 0; i < NUM_TEX; i++)
        mCylinderVerts[i] = NULL;

    for (i = 0; i < NUM_TEX; i++)
        mTextures[i] = NULL;

    mCurrentTexIndex = 0;
    mCurrentUVIndex = 0;

    // Load the soup can image into this texture
    IvImage* image = IvImage::CreateFromFile("image.tga");
    if (image)
    {
        mTextures[0] = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(), image->GetPixels(), kImmutableUsage);
        
        delete image;
        image = 0;
    }

    mTextures[0]->SetMagFiltering(kBilerpTexMagFilter);
    mTextures[0]->SetMinFiltering(kBilerpTexMinFilter);
    mTextures[0]->SetAddressingU(kWrapTexAddr);
    mTextures[0]->SetAddressingV(kWrapTexAddr);


    const unsigned int size = 128;
    mTextures[1] = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
        kRGB24TexFmt, size, size, NULL, kDefaultUsage);
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
            // 8x8 texel squares of black and white
            unsigned char color;
            if (((j >> 3) + (i >> 3)) % 2)
                color = 255;
            else
                color = 0;

            // set all three components to the same value
            tempPixels[0] = tempPixels[1] = tempPixels[2] = color;
            
            tempPixels += 3;
        }
    }

    mTextures[1]->EndLoadData(0);

    mHorizontalWrap = true;
    mVerticalWrap = true;

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShader"));

    mShader->GetUniform("Texture")->SetValue(mTextures[0]);

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    CreateCylinderVertexArrays();
}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mCylinderIndices);

    unsigned int i;
    for (i = 0; i < NUM_UVS; i++)
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCylinderVerts[i]);
    
    for (i = 0; i < NUM_UVS; i++)
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mTextures[i]); 

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

    // change texture
    if (IvGame::mGame->mEventHandler->IsKeyDown('t'))
    {
        mCurrentTexIndex = (mCurrentTexIndex + 1) % NUM_TEX;

        mShader->GetUniform("Texture")->SetValue(mTextures[mCurrentTexIndex]);

        IvGame::mGame->mEventHandler->KeyUp('t');
    }

    // change texture UVs
    if (IvGame::mGame->mEventHandler->IsKeyDown('c'))
    {
        mCurrentUVIndex = (mCurrentUVIndex + 1) % NUM_UVS;
        IvGame::mGame->mEventHandler->KeyUp('c');
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('h'))
    {
        int i;

        mHorizontalWrap = !mHorizontalWrap;

        for (i = 0; i < NUM_TEX; i++)
            mTextures[i]->SetAddressingU(mHorizontalWrap ? kWrapTexAddr : kClampTexAddr);

        IvGame::mGame->mEventHandler->KeyUp('h');
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('v'))
    {
        int i;

        mVerticalWrap = !mVerticalWrap;

        for (i = 0; i < NUM_TEX; i++)
            mTextures[i]->SetAddressingV(mVerticalWrap ? kWrapTexAddr : kClampTexAddr);

        IvGame::mGame->mEventHandler->KeyUp('v');
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
    DrawCylinderVertexArrays();

}   // End of Player::Render()


//-------------------------------------------------------------------------------
// @ Player::DrawCylinderVertexArrays()
//-------------------------------------------------------------------------------
// Draw vertex arrays for a cylinder centered around the origin
//-------------------------------------------------------------------------------
void
Player::DrawCylinderVertexArrays()
{
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mCylinderVerts[mCurrentUVIndex],
        mCylinderIndices);
} // End of Player::DrawCylinderVertexArrays()
    
//-------------------------------------------------------------------------------
// @ Player::CreateCylinderVertexArrays()
//-------------------------------------------------------------------------------
// Create vertex arrays for a cylinder centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreateCylinderVertexArrays()                                    
{
    // Creates a grid of points, shaped into a cylinder.  In order to avoid a
    // texturing anomaly, we cannot simply share the vertical seam edge vertices
    // They must be duplicated; one copy must have a U-coord of 0.0, the other a
    // U-coord of 1.0f
    const unsigned int steps = 32;
    mCylinderVerts[0] = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kTCPFormat, (steps + 1) * steps, NULL, kDefaultUsage);
    mCylinderVerts[1] = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kTCPFormat, (steps + 1) * steps, NULL, kDefaultUsage);

    IvTCPVertex* tempVerts0 = (IvTCPVertex*)(mCylinderVerts[0]->BeginLoadData());
    IvTCPVertex* tempVerts1 = (IvTCPVertex*)(mCylinderVerts[1]->BeginLoadData());

    IvTCPVertex* tempVertsCopy0 = tempVerts0;

    // temporary pointers that can be stepped along the arrays
    const float phiIncrement = kPI / (steps - 1);
    const float thetaIncrement = kTwoPI / steps;
    unsigned int i,j;

    // A double loop, walking around and down the cylinder
    for (j = 0; j <= steps; j++)
    {
        float theta = thetaIncrement * j;
        float u = j / (float)steps;

        float sinTheta, cosTheta;
        IvSinCos(theta, sinTheta, cosTheta);

        for (i = 0; i < steps; i++)
        {
            float phi = -kHalfPI + phiIncrement * i;
            float v = i / (float)(steps - 1);

            if (i == 0)
            {
                tempVerts0->position = IvVector3(0.0f, 0.0f, -mRadius);
            }
            else if (i == (steps - 1))
            {
                tempVerts0->position = IvVector3(0.0f, 0.0f, mRadius);
            }
            else
            {
                tempVerts0->position = IvVector3(mRadius * cosTheta, mRadius * sinTheta, mRadius * IvSin(phi));
            }

            tempVerts0->texturecoord = IvVector2(u, v);

            tempVerts0->color.mAlpha = 
            tempVerts0->color.mRed = 
            tempVerts0->color.mGreen = 
            tempVerts0->color.mBlue = 255; 

            tempVerts0++;
        }
    }

    memcpy(tempVerts1, tempVertsCopy0, (steps + 1) * steps * sizeof(IvTCPVertex));
    for (i = 0; i < ((steps + 1) * steps); i++)
    {
        tempVerts1->texturecoord *= 2.0f;
        tempVerts1++;
    }

    mCylinderVerts[0]->EndLoadData();
    mCylinderVerts[1]->EndLoadData();

    // Create index arrays - just a 32x31-quad mesh of triangles
    // Each of the 32 strips has 31 * 2 triangles plus two dummy indices
    // This means that there are 31 * 2 + 2 + 2 (two extra to start the
    // strip, and two extra to end the previous strip) indices in each
    // strip, although we can avoid two indices in the first strip, as
    // there is no previous strip to be ended in that case.  Thus,
    // 64 + 66 * 31 indices for the entire cylinder
    unsigned int cylinderIndexCount = steps * 2 + (steps - 1) * (steps * 2 + 2);

    mCylinderIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(
        cylinderIndexCount, NULL, kDefaultUsage);

    unsigned int* tempIndices = (unsigned int*)mCylinderIndices->BeginLoadData();

    for (j = 0; j < steps; j++)
    {
        unsigned int baseIndex0 = steps * j;
        unsigned int baseIndex1 = steps * (j + 1);

        // restart the strip by doubling the last and next indices
        if (j != 0)
        {
            *(tempIndices++) = tempIndices[-1];
            *(tempIndices++) = baseIndex0;
        }

        unsigned int i;
        for (i = 0; i < steps; i++)
        {
            *(tempIndices++) = baseIndex0;
            *(tempIndices++) = baseIndex1;

            baseIndex0++;
            baseIndex1++;
        }
    }

    mCylinderIndices->EndLoadData();

}   // End of Player::CreateCylinderVertexArrays()



