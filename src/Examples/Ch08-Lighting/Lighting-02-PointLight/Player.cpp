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
    mRadius = 2.0f;

    mSphereIndices = NULL;

    mSphereVerts = NULL;

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "pointShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "pointShader"));

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    mShader->GetUniform("pointLightIntensity")->SetValue(IvVector3(1.0f, 1.0f, 1.0f), 0);

    mShader->GetUniform("pointLightAttenuation")->SetValue(IvVector3(1.0f, 0.0f, 0.0f), 0);
    
    mLightPos = IvVector3(0.0f, -10.0f, 0.0f);

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
    float x = 0.0f, y = 0.0f, z = 0.0f;
    static bool lightPosChanged = true;

    // set up translation
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        z -= 3.0f*dt;
        lightPosChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        z += 3.0f*dt;
        lightPosChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        x += 3.0f*dt;
        lightPosChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        x -= 3.0f*dt;
        lightPosChanged = true;
    }

    if (lightPosChanged)
    {       
        mLightPos += IvVector3(x, y, z);

        lightPosChanged = false;
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('0'))
    {
        mShader->GetUniform("pointLightAttenuation")->SetValue(
            IvVector3(1.0f, 0.0f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('1'))
    {
        mShader->GetUniform("pointLightAttenuation")->SetValue(
            IvVector3(0.0f, 0.25f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('2'))
    {
        mShader->GetUniform("pointLightAttenuation")->SetValue(
            IvVector3(0.0f, 0.0f, 0.0125f), 0);
    }

    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mLightPos = IvVector3(0.0f, -10.0f, 0.0f);
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
    for (j = -1; j <= 1; j++)
    {
        for (i = -1; i <= 1; i++)
        {
            transform(0, 3) = 5.0f * i;
            transform(2, 3) = 5.0f * j;

            IvMatrix44 inv = transform;
            inv.AffineInverse();

            mShader->GetUniform("pointLightPosition")->SetValue(mLightPos, 0);

            ::IvSetWorldMatrix(transform);
            mShader->GetUniform("modelMatrix")->SetValue(transform, 0);

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
        kNPFormat, verts, NULL, kDefaultUsage);

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

        float red = fmod(4.0f * j / (float)(steps - 1), 1.0f);

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
        CreateIndexBuffer(sphereIndexCount, NULL, kDefaultUsage);

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



