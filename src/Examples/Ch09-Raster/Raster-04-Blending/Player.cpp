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

IvTexture* CreateTextureFromFile(const char* file)
{
    IvTexture* tex = NULL;

    // Load the soup can image into this texture
    IvImage* image = IvImage::CreateFromFile(file);
    if (image)
    {
        tex = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight());

        unsigned char* pixels = (unsigned char*)(tex->BeginLoadData(0));

        memcpy(pixels, image->GetPixels(), 
            image->GetBytesPerPixel() * image->GetWidth() * image->GetHeight());

        tex->EndLoadData(0);

        delete image;
        image = 0;
    }

    tex->SetMagFiltering(kBilerpTexMagFilter);
    tex->SetMinFiltering(kBilerpTexMinFilter);
    tex->SetAddressingU(kWrapTexAddr);
    tex->SetAddressingV(kWrapTexAddr);

    return tex;
}

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

    mCylinderVerts = 0;
    mCylinderIndices = 0;

    mBlendMode = kOpacityBlendFunc;

    unsigned int i;

    mBaseTexture = NULL;

    for (i = 0; i < NUM_BLEND_TEX; i++)
        mBlendTextures[i] = NULL;

    mCurrentBlendTexIndex = 0;

    mBaseTexture = CreateTextureFromFile("image.tga");
    mBlendTextures[0] = CreateTextureFromFile("fence.tga");
    mBlendTextures[1] = CreateTextureFromFile("darkdots.tga");
    mBlendTextures[2] = CreateTextureFromFile("glowdots.tga");

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShader"));

    mTextureUniform = mShader->GetUniform("Texture");

    mTextureUniform->SetValue(mBaseTexture);

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    CreateCylinder();
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

    // change texture
    if (IvGame::mGame->mEventHandler->IsKeyDown('t'))
    {
        mCurrentBlendTexIndex = (mCurrentBlendTexIndex + 1) % NUM_BLEND_TEX;
        IvGame::mGame->mEventHandler->KeyUp('t');
    }

    // change blending mode
    if (IvGame::mGame->mEventHandler->IsKeyDown('b'))
    {
        mBlendMode = (IvBlendFunc)((mBlendMode + 1) % kBlendFuncCount);

        IvGame::mGame->mEventHandler->KeyUp('b');
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
    
    ::IvSetWorldMatrix(transform);

    // disable blending
    IvRenderer::mRenderer->SetBlendFunc(kNoBlendFunc);

    // set base texture
    mTextureUniform->SetValue(mBaseTexture);

    // draw inner, opaque cylinder
    DrawCylinder();

    // push transform with larger scale for outside cylinder
    IvMatrix44 transformOutside(mRotate);

    float outerScale = mScale * 1.2f;

    transformOutside(0,0) *= outerScale;
    transformOutside(1,0) *= outerScale;
    transformOutside(2,0) *= outerScale;
    transformOutside(0,1) *= outerScale;
    transformOutside(1,1) *= outerScale;
    transformOutside(2,1) *= outerScale;
    transformOutside(0,2) *= outerScale;
    transformOutside(1,2) *= outerScale;
    transformOutside(2,2) *= outerScale;
    transformOutside(0,3) = mTranslate.GetX();
    transformOutside(1,3) = mTranslate.GetY();
    transformOutside(2,3) = mTranslate.GetZ();
    
    ::IvSetWorldMatrix(transformOutside);

    IvRenderer::mRenderer->SetBlendFunc(mBlendMode);

    // set blending texture
    mTextureUniform->SetValue(mBlendTextures[mCurrentBlendTexIndex]);

    // draw larger, blended cylinder
    DrawCylinder();

}   // End of Player::Render()



//-------------------------------------------------------------------------------
// @ Player::DrawCylinder()
//-------------------------------------------------------------------------------
// Draw vertex arrays for a cylinder centered around the origin
//-------------------------------------------------------------------------------
void
Player::DrawCylinder()
{
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mCylinderVerts,
        mCylinderIndices);
} // End of Player::DrawCylinder()
    
//-------------------------------------------------------------------------------
// @ Player::CreateCylinder()
//-------------------------------------------------------------------------------
// Create vertex arrays for a cylinder centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreateCylinder()                                    
{
    // Creates a grid of points, shaped into a cylinder.  In order to avoid a
    // texturing anomaly, we cannot simply share the vertical seam edge vertices
    // They must be duplicated; one copy must have a U-coord of 0.0, the other a
    // U-coord of 1.0f
    const unsigned int steps = 32;
    mCylinderVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kTCPFormat, (steps + 1) * steps);

    IvTCPVertex* tempVerts0 = (IvTCPVertex*)(mCylinderVerts->BeginLoadData());

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

    mCylinderVerts->EndLoadData();

    // Create index arrays - just a 32x31-quad mesh of triangles
    // Each of the 32 strips has 31 * 2 triangles plus two dummy indices
    // This means that there are 31 * 2 + 2 + 2 (two extra to start the
    // strip, and two extra to end the previous strip) indices in each
    // strip, although we can avoid two indices in the first strip, as
    // there is no previous strip to be ended in that case.  Thus,
    // 64 + 66 * 31 indices for the entire cylinder
    unsigned int cylinderIndexCount = steps * 2 + (steps - 1) * (steps * 2 + 2);

    mCylinderIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(
        cylinderIndexCount);

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

}   // End of Player::CreateCylinder()



