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
    mRadius = 5.0f;

    mSphereIndices = nullptr;

    mSphereVerts = nullptr;

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "directionalShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "directionalShader"));

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    IvVector3 radiance(1.0f, 1.0f, 1.0f);
    mShader->GetUniform("dirLightRadiance")->SetValue(radiance, 0);

    mRotate.Identity();

    mLightDir = IvVector3(1.0f, 0.0f, 0.0f);

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
    float r = 0.0f;
    static bool lightDirChanged = true;

    // set up rotate
    if (IvGame::mGame->mEventHandler->IsKeyDown('o'))
    {
        r -= kPI*0.25f*dt;
        lightDirChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('u'))
    {
        r += kPI*0.25f*dt;
        lightDirChanged = true;
    }

    if (lightDirChanged)
    {
        IvMatrix44 rotate;
        rotate.RotationZ(r);
       
        mLightDir = rotate.Transform(mLightDir);
        mLightDir.Normalize();

        lightDirChanged = false;
    }

    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
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
    IvMatrix44 transform;

    int i,j;
    for (j = 0; j <=0 ; j++)
    {
        for (i = 0; i <= 0; i++)
        {
            transform(0, 3) = 5.0f * i;
            transform(2, 3) = 5.0f * j;
            
            IvVector3 lightDir = transform.AffineInverse().Transform(mLightDir);

            mShader->GetUniform("dirLightPosition")->SetValue(lightDir, 0);

            ::IvSetWorldMatrix(transform);

            // draw geometry
            DrawSphere();
        }
    }

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
        kNPFormat, verts, nullptr, kDefaultUsage);

    // temporary pointers that can be stepped along the arrays
    IvNPVertex* tempVerts = (IvNPVertex*)(mSphereVerts->BeginLoadData());

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

            IvVector3 pos(mRadius * cosTheta * cosPhi, mRadius * sinTheta * cosPhi, mRadius * sinPhi);

            tempVerts->position = pos;

            pos.Normalize();

            tempVerts->normal = pos;

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
        CreateIndexBuffer(sphereIndexCount, nullptr, kDefaultUsage);

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



