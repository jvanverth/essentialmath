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
#include <IvIndexBuffer.h>
#include <IvMath.h>
#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvResourceManager.h>
#include <IvShaderProgram.h>
#include <IvUniform.h>
#include <IvVector3.h>
#include <IvVertexBuffer.h>
#include <IvVertexShader.h>


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

    mSharpStripIndices = NULL;
    mSmoothStripIndices = NULL;

    mSharpStripVerts = NULL;
    mSmoothStripVerts = NULL;

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "vertColorShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "vertColorShader"));

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    CreateStrips();
}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mSmoothStripIndices);
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mSharpStripIndices);

    IvRenderer::mRenderer->GetResourceManager()->Destroy(mSmoothStripVerts);
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mSharpStripVerts);
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
    DrawStrips();

}   // End of Player::Render()


//-------------------------------------------------------------------------------
// @ Player::DrawStrips()
//-------------------------------------------------------------------------------
// Draw vertex arrays for a sphere centered around the origin
//-------------------------------------------------------------------------------
void
Player::DrawStrips()
{
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mSharpStripVerts, mSharpStripIndices);
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mSmoothStripVerts, mSmoothStripIndices);
} // End of Player::DrawStrips()
    
static void AddStripPair(IvCPVertex*& verts, const IvVector3& basePt, 
                  const IvVector3& dirVec, const IvColor& color)
{
    verts->position = basePt;
    verts->color = color;
    verts++;

    verts->position = basePt + dirVec;
    verts->color = color;
    verts++;
}

//-------------------------------------------------------------------------------
// @ Player::CreateStrips()
//-------------------------------------------------------------------------------
// Create vertex arrays for strips centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreateStrips()                                    
{
    const unsigned int stripQuads = 4;

    mSmoothStripVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kCPFormat, stripQuads * 2, NULL, kDefaultUsage);
    mSharpStripVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kCPFormat, stripQuads * 4, NULL, kDefaultUsage);

    // temporary pointers that can be stepped along the arrays
    IvCPVertex* tempSmoothVerts = (IvCPVertex*)(mSmoothStripVerts->BeginLoadData());
    IvCPVertex* tempSharpVerts = (IvCPVertex*)(mSharpStripVerts->BeginLoadData());

    const IvVector3 dir(0.0f, -2.0f, 0.0f);
    IvVector3 pos(-2.0f * (stripQuads>>1), -1.0f, 0.0f);

    IvColor color = {0, 0, 255, 255};

    unsigned int i;
    for (i = 0; i < stripQuads; i++)
    {
        color.mRed = 
            color.mGreen = 50 * i;
        color.mBlue = 255 - 50 * i;

        AddStripPair(tempSmoothVerts, pos + dir, dir, color);

        AddStripPair(tempSharpVerts, pos - dir, dir, color);
        
        pos.SetX( pos.GetX() + 2.0f);

        AddStripPair(tempSharpVerts, pos - dir, dir, color);
    }

    mSmoothStripVerts->EndLoadData();
    mSharpStripVerts->EndLoadData();

    mSmoothStripIndices = IvRenderer::mRenderer->GetResourceManager()->
        CreateIndexBuffer(stripQuads * 2, NULL, kDefaultUsage);
    mSharpStripIndices = IvRenderer::mRenderer->GetResourceManager()->
        CreateIndexBuffer(stripQuads * 6 - 2, NULL, kDefaultUsage);

    unsigned int* tempSmoothIndices = (unsigned int*)(mSmoothStripIndices->BeginLoadData());
    unsigned int* tempSharpIndices = (unsigned int*)(mSharpStripIndices->BeginLoadData());

    *(tempSmoothIndices++) = 0;
    *(tempSmoothIndices++) = 1;

    *(tempSharpIndices++) = 0;
    *(tempSharpIndices++) = 1;

    for (i = 0; i < stripQuads; i++)
    {

        // close out the quad and (if not at the end) open the next
        *(tempSharpIndices++) = 4 * i + 2;
        *(tempSharpIndices++) = 4 * i + 3;

        if (i < (stripQuads-1))
        {
            *(tempSharpIndices++) = 4 * i + 3;
            *(tempSharpIndices++) = 4 * i + 4;
            *(tempSharpIndices++) = 4 * i + 4;
            *(tempSharpIndices++) = 4 * i + 5;

            // close out the quad
            *(tempSmoothIndices++) = 2 * i + 2;
            *(tempSmoothIndices++) = 2 * i + 3;
        }
    }

    mSmoothStripIndices->EndLoadData();
    mSharpStripIndices->EndLoadData();

}   // End of Player::CreateSphere()



