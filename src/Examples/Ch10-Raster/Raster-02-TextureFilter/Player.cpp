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

    // Load the soup can image into this texture
    IvImage* image = IvImage::CreateFromFile("euro.tga");
    if (image)
    {
        mTexture = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(), image->GetPixels(),
            kImmutableUsage);

        delete image;
        image = 0;
    }

    mTexture->SetMagFiltering(kBilerpTexMagFilter);
    mTexture->SetMinFiltering(kBilerpTexMinFilter);
    mTexture->SetAddressingU(kClampTexAddr);
    mTexture->SetAddressingV(kClampTexAddr);

    const unsigned int size = 128;
    mCheckerTexture = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
        kRGB24TexFmt, size, size, NULL, kDynamicUsage);
    mCheckerTexture->SetMagFiltering(kBilerpTexMagFilter);
    mCheckerTexture->SetMinFiltering(kBilerpTexMinFilter);
    mCheckerTexture->SetAddressingU(kClampTexAddr);
    mCheckerTexture->SetAddressingV(kClampTexAddr);

    unsigned char* pixels = (unsigned char*)(mCheckerTexture->BeginLoadData(0));

    unsigned char* tempPixels = pixels;

    unsigned int i,j;
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

    mCheckerTexture->EndLoadData(0);

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShader"));

    mShader->GetUniform("Texture")->SetValue(mTexture);

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    CreateQuad();

    mBilerp = false;
    mCheckerboard = false;

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
    rotate.RotationY(r);
    mRotate = rotate*mRotate;
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
        mScale = 1.0f;
    }

    // switch filtering mode
    if (IvGame::mGame->mEventHandler->IsKeyDown('f'))
    {
        mBilerp = !mBilerp;

        IvGame::mGame->mEventHandler->KeyUp('f');
    }

    // switch texture
    if (IvGame::mGame->mEventHandler->IsKeyDown('t'))
    {
        mCheckerboard = !mCheckerboard;

        IvGame::mGame->mEventHandler->KeyUp('t');
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
    transform(0,3) = 0.0f;
    transform(1,3) = 0.0f;
    transform(2,3) = 0.0f;
    
    ::IvSetWorldMatrix(transform);

    // Set the desired texture
    if (mCheckerboard)
        mShader->GetUniform("Texture")->SetValue(mCheckerTexture);
    else
        mShader->GetUniform("Texture")->SetValue(mTexture);

    // Set the desired filtering mode
    if (mBilerp)
    {
        mCheckerTexture->SetMagFiltering(kBilerpTexMagFilter);
        mCheckerTexture->SetMinFiltering(kBilerpTexMinFilter);
        mTexture->SetMagFiltering(kBilerpTexMagFilter);
        mTexture->SetMinFiltering(kBilerpTexMinFilter);
    }
    else
    {
        mCheckerTexture->SetMagFiltering(kNearestTexMagFilter);
        mCheckerTexture->SetMinFiltering(kNearestTexMinFilter);
        mTexture->SetMagFiltering(kNearestTexMagFilter);
        mTexture->SetMinFiltering(kNearestTexMinFilter);
    }

    // draw geometry
    DrawQuad();

}   // End of Player::Render()


//-------------------------------------------------------------------------------
// @ Player::DrawQuadVertexArrays()
//-------------------------------------------------------------------------------
// Draw vertex arrays for a quad centered around the origin
//-------------------------------------------------------------------------------
void
Player::DrawQuad()
{
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mQuadVerts, mQuadIndices);
} // End of Player::DrawQuadVertexArrays()
    
//-------------------------------------------------------------------------------
// @ Player::CreateQuadVertexArrays()
//-------------------------------------------------------------------------------
// Create vertex arrays for a quad centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreateQuad()                                    
{
    const float size = 3.0f;

    mQuadVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kTCPFormat, 4, NULL, kDynamicUsage);

    // temporary pointers that can be stepped along the arrays
    IvTCPVertex* tempVerts = (IvTCPVertex*)(mQuadVerts->BeginLoadData());

    tempVerts->texturecoord = IvVector2(0.0f, 0.0f);
    tempVerts->position = IvVector3(-size, 0.0f, -size);
    tempVerts++;

    tempVerts->texturecoord = IvVector2(1.0f, 0.0f);
    tempVerts->position = IvVector3(size, 0.0f, -size);
    tempVerts++;

    tempVerts->texturecoord = IvVector2(0.0f, 1.0f);
    tempVerts->position = IvVector3(-size, 0.0f, size);
    tempVerts++;

    tempVerts->texturecoord = IvVector2(1.0f, 1.0f);
    tempVerts->position = IvVector3(size, 0.0f, size);
    tempVerts++;

    mQuadVerts->EndLoadData();

    mQuadIndices = IvRenderer::mRenderer->GetResourceManager()->
        CreateIndexBuffer(4, NULL, kDynamicUsage);

    unsigned int* tempIndices = (unsigned int*)(mQuadIndices->BeginLoadData());

    int j;
    for (j = 0; j < 4; j++)
        *(tempIndices++) = j;

    mQuadIndices->EndLoadData();
}   // End of Player::CreateQuad()


