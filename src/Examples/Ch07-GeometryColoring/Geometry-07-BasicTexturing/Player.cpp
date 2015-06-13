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

    mRadius = 3.0f;

    mCylinderIndices = NULL;

    unsigned int i;
    for (i = 0; i < NUM_UVS; i++)
        mCylinderVerts[i] = NULL;

    for (i = 0; i < NUM_TEX; i++)
        mTextures[i] = NULL;

    mCurrentTexIndex = 0;
    mCurrentUVIndex = 0;

    // Load the soup can image into this texture
    IvImage* image = IvImage::CreateFromFile("image.tga");
    if (image)
    {
        void* pixels = image->GetPixels();
        mTextures[0] = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(), pixels, kDefaultUsage);
        
        //unsigned char* pixels = (unsigned char*)(mTextures[0]->BeginLoadData(0));
        //memcpy(pixels, image->GetPixels(), image->GetBytesPerPixel()*image->GetWidth()*image->GetHeight());
        //mTextures[0]->EndLoadData(0);


        delete image;
        image = 0;
    }

    mTextures[0]->SetMagFiltering(kBilerpTexMagFilter);
    mTextures[0]->SetMinFiltering(kBilerpTexMinFilter);
    mTextures[0]->SetAddressingU(kWrapTexAddr);
    mTextures[0]->SetAddressingV(kWrapTexAddr);

    const unsigned int size = 128;
    mTextures[1] = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(kRGB24TexFmt,
                                                                              size, size,
                                                                              NULL, kDefaultUsage);
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
    
    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShader"));

	IvUniform* texUniform = mShader->GetUniform("Texture");
    if (texUniform)
		texUniform->SetValue(mTextures[0]);

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
    size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();

    // Creates a grid of points, shaped into a cylinder.  In order to avoid a
    // texturing anomaly, we cannot simply share the vertical seam edge vertices
    // They must be duplicated; one copy must have a U-coord of 0.0, the other a
    // U-coord of 1.0f
    const unsigned int steps = 32;
    const unsigned int numVerts = (steps + 1) * steps;
    IvTCPVertex* tempVerts0 = (IvTCPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kTCPFormat] * numVerts);
    IvTCPVertex* tempVerts1 = (IvTCPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kTCPFormat] * numVerts);
    
    // temporary pointers that can be stepped along the arrays
    const float phiIncrement = kPI / (steps - 1);
    const float thetaIncrement = kTwoPI / steps;
    unsigned int i,j;

    // A double loop, walking around and down the cylinder
    IvTCPVertex* tempVertsPtr = tempVerts0;
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
                tempVertsPtr->position = IvVector3(0.0f, 0.0f, -mRadius);
            }
            else if (i == (steps - 1))
            {
                tempVertsPtr->position = IvVector3(0.0f, 0.0f, mRadius);
            }
            else
            {
                tempVertsPtr->position = IvVector3(mRadius * cosTheta, mRadius * sinTheta, mRadius * IvSin(phi));
            }

            tempVertsPtr->texturecoord = IvVector2(u, v);

            tempVertsPtr->color.mAlpha =
            tempVertsPtr->color.mRed =
            tempVertsPtr->color.mGreen =
            tempVertsPtr->color.mBlue = 255;

            tempVertsPtr++;
        }
    }

    memcpy(tempVerts1, tempVerts0, (steps + 1) * steps * sizeof(IvTCPVertex));
    tempVertsPtr = tempVerts1;
    for (i = 0; i < ((steps + 1) * steps); i++)
    {
        tempVertsPtr->texturecoord *= 2.0f;
        tempVertsPtr++;
    }

    mCylinderVerts[0] = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kTCPFormat,
                                                                                        numVerts,
                                                                                        tempVerts0, kDefaultUsage);
    mCylinderVerts[1] = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kTCPFormat,
                                                                                        numVerts,
                                                                                        tempVerts1, kDefaultUsage);
    IvStackAllocator::mScratchAllocator->Reset(currentOffset);

    // Create index arrays - just a 32x31-quad mesh of triangles
    // Each of the 32 strips has 31 * 2 triangles plus two dummy indices
    // This means that there are 31 * 2 + 2 + 2 (two extra to start the
    // strip, and two extra to end the previous strip) indices in each
    // strip, although we can avoid two indices in the first strip, as
    // there is no previous strip to be ended in that case.  Thus,
    // 64 + 66 * 31 indices for the entire cylinder
    unsigned int cylinderIndexCount = steps * 2 + (steps - 1) * (steps * 2 + 2);
    UInt32* tempIndices = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32) * cylinderIndexCount);

    UInt32* tempIndexPtr = tempIndices;
    for (j = 0; j < steps; j++)
    {
        unsigned int baseIndex0 = steps * j;
        unsigned int baseIndex1 = steps * (j + 1);
        unsigned int lastBaseIndex1 = baseIndex1;

        // restart the strip by doubling the last and next indices
        if (j != 0)
        {
            *(tempIndexPtr++) = lastBaseIndex1;
            *(tempIndexPtr++) = baseIndex0;
        }

        unsigned int i;
        for (i = 0; i < steps; i++)
        {
            *(tempIndexPtr++) = baseIndex0;
            *(tempIndexPtr++) = baseIndex1;
            lastBaseIndex1 = baseIndex1;
            
            baseIndex0++;
            baseIndex1++;
        }
    }

    mCylinderIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(
                                                                cylinderIndexCount, tempIndices, kDefaultUsage);

    IvStackAllocator::mScratchAllocator->Reset(currentOffset);

}   // End of Player::CreateCylinderVertexArrays()



