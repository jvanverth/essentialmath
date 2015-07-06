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
    // Rotate the quads so that they are initially oblique, showing multiple
    // mipmap levels across their surfaces (or extreme aliasing if mipmapping is
    // not enabled)
    mRotate.RotationY(-kPI * 0.35f);

    mScale = 1.0f;

    // Create a texture with a solid color per mipmap level.  This will make it
    // very clear what mipmap level is being rendered, as each mipmap level has
    // its own color.

    // create a texture "name" to reference later
    mMipmapLevelBandsTex = IvRenderer::mRenderer->GetResourceManager()->CreateMipmappedTexture(
        kRGB24TexFmt, 256, 256, NULL, 9, kDefaultUsage);

    mMipmapLevelBandsTex->SetMagFiltering(kBilerpTexMagFilter);
    mMipmapLevelBandsTex->SetMinFiltering(kBilerpTexMinFilter);
    mMipmapLevelBandsTex->SetAddressingU(kWrapTexAddr);
    mMipmapLevelBandsTex->SetAddressingV(kWrapTexAddr);

    // Cannot use GLU to generate mipmap levels, as each mipmap level has nothing
    // to do with the previous mipmap level.  Each mipmap level is simply a solid
    // color
    unsigned int i;
    for (i = 0; i < 9; i++)
    {
        IvTexColorRGB* texels = (IvTexColorRGB*)mMipmapLevelBandsTex->BeginLoadData(i);

        unsigned int size = 256 >> i;

        unsigned char red = (i & 0x1) ? 255 : 127;
        unsigned char green = (i & 0x2) ? 255 : 127;
        unsigned char blue = (i & 0x4) ? 255 : 127;
        
        unsigned int j;
        for (j  = 0; j < size * size; j++)
        {
            texels[j].r = red;
            texels[j].g = green;
            texels[j].b = blue;
        }

        mMipmapLevelBandsTex->EndLoadData(i);
    }

    // Create a classic checkerboard pattern to show off aliasing
    // create a texture "name" to reference later
    mCheckerBoardTex = IvRenderer::mRenderer->GetResourceManager()->CreateMipmappedTexture(
        kRGB24TexFmt, 256, 256, NULL, 5, kDefaultUsage);

    mCheckerBoardTex->SetMagFiltering(kBilerpTexMagFilter);
    mCheckerBoardTex->SetMinFiltering(kBilerpTexMinFilter);
    mCheckerBoardTex->SetAddressingU(kWrapTexAddr);
    mCheckerBoardTex->SetAddressingV(kWrapTexAddr);

    int size = 256;
    int shift = 3;
    for (unsigned int k = 0; k < 5; ++k)
    {
        IvTexColorRGB* texels = (IvTexColorRGB*)mCheckerBoardTex->BeginLoadData(k);
        unsigned int j;
        for (j  = 0; j < size; j++)
        {
            for (i  = 0; i < size; i++)
            {
                unsigned int pixelIndex = j * size + i;
                
                if (k == 4)
                {
                    // medium gray
                    texels[pixelIndex].r = texels[pixelIndex].g
                    = texels[pixelIndex].b = 180;
                }
                else
                {
                    // checkerboard patterns
                    if (((j >> shift) + (i >> shift)) & 0x1)
                    {
                        texels[pixelIndex].r = texels[pixelIndex].g 
                            = texels[pixelIndex].b = 255;
                    }
                    else
                    {
                        texels[pixelIndex].r = texels[pixelIndex].g 
                            = texels[pixelIndex].b = 0;
                    }
                }
            }
        }
        mCheckerBoardTex->EndLoadData(k);
        size >>= 1;
        shift--;
    }

    mFilter = FILTER_BILERP;

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "textureShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "textureShader"));

    mTextureUniform = mShader->GetUniform("Texture");
        
    mTextureUniform->SetValue(mCheckerBoardTex);

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    CreateQuad();
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
        mRotate.RotationY(-kPI * 0.35f);
        mScale = 1.0f;
    }

    // cycle through the filtering modes - note that we need to set the filtering modes for
    // both textures, as these are stored as a part of BindTexture
    if (IvGame::mGame->mEventHandler->IsKeyDown('f'))
    {
        switch (mFilter)
        {
        case FILTER_BILERP:
            // Switch to mipmap with bilerp across the selected level
            mMipmapLevelBandsTex->SetMinFiltering(kBilerpMipmapNearestTexMinFilter);
            mCheckerBoardTex->SetMinFiltering(kBilerpMipmapNearestTexMinFilter);
            mFilter = FILTER_MIPMAP;
            break;
        case FILTER_MIPMAP:
            // Switch to trilinear filtering
            mMipmapLevelBandsTex->SetMinFiltering(kBilerpMipmapLerpTexMinFilter);
            mCheckerBoardTex->SetMinFiltering(kBilerpMipmapLerpTexMinFilter);
            mFilter = FILTER_TRILERP;
            break;
        case FILTER_TRILERP:
            // Switch to non-mipmapped bilinear filtering
            mMipmapLevelBandsTex->SetMinFiltering(kBilerpTexMinFilter);
            mCheckerBoardTex->SetMinFiltering(kBilerpTexMinFilter);
            mFilter = FILTER_BILERP;
            break;
        };

        IvGame::mGame->mEventHandler->KeyUp('f');
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
    transform(1,3) = -1.5f;
    transform(2,3) = 0.0f;
    
    ::IvSetWorldMatrix(transform);

    // Render a quad with the mipmap band texture
    mTextureUniform->SetValue(mMipmapLevelBandsTex);

    // draw geometry
    DrawQuad();

    transform(1,3) = 1.5f;
    
    ::IvSetWorldMatrix(transform);

    // Render a quad with the checkerboard
    mTextureUniform->SetValue(mCheckerBoardTex);

    // draw geometry
    DrawQuad();
} // End of Player::Render()


//-------------------------------------------------------------------------------
// @ Player::DrawQuads()
//-------------------------------------------------------------------------------
// Draw a quad centered around the origin
//-------------------------------------------------------------------------------
void 
Player::DrawQuad()      
{   
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mQuadVerts, mQuadIndices);
}


//-------------------------------------------------------------------------------
// @ Player::CreateQuads()
//-------------------------------------------------------------------------------
// Create a quad centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreateQuad()      
{   
    const float size = 1.0f;

    mQuadVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kTCPFormat, 4, NULL, kDefaultUsage);

    // temporary pointers that can be stepped along the arrays
    IvTCPVertex* tempVerts = (IvTCPVertex*)(mQuadVerts->BeginLoadData());

    tempVerts->texturecoord = IvVector2(0.0f, 0.0f);
    tempVerts->position = IvVector3(0.0f, -size, -size);
    tempVerts++;

    tempVerts->texturecoord = IvVector2(1.0f, 0.0f);
    tempVerts->position = IvVector3(0.0f, size, -size);
    tempVerts++;

    tempVerts->texturecoord = IvVector2(0.0f, 1.0f);
    tempVerts->position = IvVector3(0.0f, -size, size);
    tempVerts++;

    tempVerts->texturecoord = IvVector2(1.0f, 1.0f);
    tempVerts->position = IvVector3(0.0f, size, size);
    tempVerts++;

    mQuadVerts->EndLoadData();

    mQuadIndices = IvRenderer::mRenderer->GetResourceManager()->
        CreateIndexBuffer(4, NULL, kDefaultUsage);

    unsigned int* tempIndices = (unsigned int*)(mQuadIndices->BeginLoadData());

    int j;
    for (j = 0; j < 4; j++)
        *(tempIndices++) = j;

    mQuadIndices->EndLoadData();
}
