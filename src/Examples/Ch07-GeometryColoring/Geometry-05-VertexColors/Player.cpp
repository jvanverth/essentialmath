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

    mRadius = 3.0f;

    mSphereIndices = NULL;

    mSphereVerts = NULL;

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "vertColorShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "vertColorShader"));

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    CreateSphere();
}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mSphereIndices);

    IvRenderer::mRenderer->GetResourceManager()->Destroy(mSphereVerts);
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
    DrawSphere();

}   // End of Player::Render()


//-------------------------------------------------------------------------------
// @ Player::DrawSphere()
//-------------------------------------------------------------------------------
// Draw vertex arrays for a sphere centered around the origin
//-------------------------------------------------------------------------------
void
Player::DrawSphere()
{
    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mSphereVerts, mSphereIndices);
} // End of Player::DrawSphere()
    
//-------------------------------------------------------------------------------
// @ Player::CreateSphere()
//-------------------------------------------------------------------------------
// Create vertex arrays for a sphere centered around the origin
//-------------------------------------------------------------------------------
void 
Player::CreateSphere()                                    
{
    // Creates a grid of points, shaped into a sphere.  This is not an
    // efficient way to create a sphere (the verts are not evenly distributed),
    // but it shows how to set up arrays of vertices and normals
    const unsigned int steps = 32;
    const unsigned int verts = steps * steps;

    mSphereVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
        kCPFormat, verts);

    // temporary pointers that can be stepped along the arrays
    IvCPVertex* tempVerts = (IvCPVertex*)(mSphereVerts->BeginLoadData());

    // A double loop, walking around and down the sphere
    const float phiIncrement = kPI / (steps - 1);
    const float thetaIncrement = kTwoPI / steps;

    unsigned int j;
    for (j = 0; j < steps; j++)
    {
        float theta = thetaIncrement * j;

        float sinTheta, cosTheta;
        IvSinCos(theta, sinTheta, cosTheta);

        unsigned int i;
        for (i = 0; i < steps; i++)
        {
            float phi = phiIncrement * i - kHalfPI;

            float sinPhi, cosPhi;
            IvSinCos(phi, sinPhi, cosPhi);

            tempVerts->position = IvVector3(mRadius * cosTheta * cosPhi,
                mRadius * sinTheta * cosPhi, mRadius * sinPhi);

            tempVerts->color.mAlpha = 255;
            tempVerts->color.mRed = (255 * j) / (steps - 1);
            tempVerts->color.mGreen = 0;
            tempVerts->color.mBlue = (255 * i) / (steps - 1);

            tempVerts++;
        }
    }

    mSphereVerts->EndLoadData();

    // Create index arrays - just a 32x31-quad mesh of triangles
    // Each of the 32 strips has 31 * 2 triangles plus two dummy indices
    // This means that there are 31 * 2 + 2 + 2 (two extra to start the
    // strip, and two extra to end the previous strip) indices in each
    // strip, although we can avoid two indices in the first strip, as
    // there is no previous strip to be ended in that case.  Thus,
    // 64 + 66 * 31 indices for the entire sphere
    const unsigned int sphereIndexCount = steps * 2 + (steps - 1) * (steps * 2 + 2);

    mSphereIndices = IvRenderer::mRenderer->GetResourceManager()->
        CreateIndexBuffer(sphereIndexCount);

    unsigned int* tempIndices = (unsigned int*)(mSphereIndices->BeginLoadData());

    for (j = 0; j < steps; j++)
    {
        unsigned int baseIndex0 = steps * j;
        unsigned int baseIndex1 = steps * ((j + 1) % steps);

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

    mSphereIndices->EndLoadData();
}   // End of Player::CreateSphere()



