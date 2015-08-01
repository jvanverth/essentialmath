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
#include <IvIndexBuffer.h>
#include <IvMath.h>
#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvResourceManager.h>
#include <IvShaderProgram.h>
#include <IvTexture.h>
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
        "texlitShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "texlitShader"));

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    mShader->GetUniform("pointLightIntensity")->SetValue(IvVector3(50.f, 50.f, 50.0f), 0);
    
    mShader->GetUniform("ambientLightColor")->SetValue(IvVector3(0.0f, 0.0f, 0.2f), 0);
    
    mLightPos = IvVector3(0.0f, -10.0f, 0.0f);
    
    mSpecularPercentage = 0.25f;
    mAmbientFactor = 0.1f;
    mEmissiveFactor = 0.0f;
    
    ComputeMaterialComponents();
    
    mColorUniform = mShader->GetUniform("splitColor");
    mColorUniform->SetValue(0.0f, 0);
    mLightPosUniform = mShader->GetUniform("pointLightPosition");
    mViewPosUniform = mShader->GetUniform("viewPosition");

    // Load the soup can image into this texture
    IvImage* image = IvImage::CreateFromFile("image.tga");
    if (image)
    {
        mTexture = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
            (image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
            image->GetWidth(), image->GetHeight(), image->GetPixels(), kImmutableUsage);

        delete image;
        image = 0;
    }

    mTexture->SetMagFiltering(kBilerpTexMagFilter);
    mTexture->SetMinFiltering(kBilerpTexMinFilter);
    mTexture->SetAddressingU(kWrapTexAddr);
    mTexture->SetAddressingV(kWrapTexAddr);

    mShader->GetUniform("Texture")->SetValue(mTexture);

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

    IvRenderer::mRenderer->GetResourceManager()->Destroy(mTexture);

}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::ComputeMaterialComponents()
//-------------------------------------------------------------------------------
// Update material values
//-------------------------------------------------------------------------------
void
Player::ComputeMaterialComponents()
{
    IvVector3 albedo3(0.7f, 0.0f, 1.0f);
    IvVector4 albedo4(0.7f, 0.0f, 1.0f, 1.0f);
    
    if (mEmissiveFactor > 1.0f)
    {
        mEmissiveFactor = 1.0f;
    }
    else if (mEmissiveFactor < 0.0f)
    {
        mEmissiveFactor = 0.0f;
    }
    mShader->GetUniform("materialEmissiveColor")->SetValue(mEmissiveFactor*albedo3, 0);
    
    if (mAmbientFactor > 1.0f)
    {
        mAmbientFactor = 1.0f;
    }
    else if (mAmbientFactor < 0.0f)
    {
        mAmbientFactor = 0.0f;
    }
    mShader->GetUniform("materialAmbientColor")->SetValue(mAmbientFactor*albedo3, 0);
    
    if (mSpecularPercentage > 1.0f)
    {
        mSpecularPercentage = 1.0f;
    }
    else if (mSpecularPercentage < 0.0f)
    {
        mSpecularPercentage = 0.0f;
    }
    mShader->GetUniform("materialDiffuseColor")->SetValue(
                                                          (1.0f - mSpecularPercentage)*albedo4, 0);
    
    float specularExp = 16.0f;
    IvVector3 specularColor(1.0f, 1.0f, 1.0f);
    specularColor *= mSpecularPercentage;
    specularColor *= (specularExp + 8.0f)/8.0f; // normalization factor
    mShader->GetUniform("materialSpecularColorExp")->SetValue(IvVector4(specularColor.x,
                                                                        specularColor.y,
                                                                        specularColor.z,
                                                                        specularExp), 0);
}

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

    bool materialChanged = false;
    if (IvGame::mGame->mEventHandler->IsKeyDown('q'))
    {
        mAmbientFactor += dt;
        materialChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('a'))
    {
        mAmbientFactor -= dt;
        materialChanged = true;
    }
    
    if (IvGame::mGame->mEventHandler->IsKeyDown('w'))
    {
        mSpecularPercentage += dt;
        materialChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('s'))
    {
        mSpecularPercentage -= dt;
        materialChanged = true;
    }
    
    if (IvGame::mGame->mEventHandler->IsKeyDown('e'))
    {
        mEmissiveFactor += dt;
        materialChanged = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('d'))
    {
        mEmissiveFactor -= dt;
        materialChanged = true;
    }
    
    if (IvGame::mGame->mEventHandler->IsKeyDown('x'))
    {
        mColorUniform->SetValue(1.0f, 0);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('c'))
    {
        mColorUniform->SetValue(0.0f, 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mLightPos = IvVector3(0.0f, -10.0f, 0.0f);
        mEmissiveFactor = 0.0f;
        mAmbientFactor = 0.1f;
        mSpecularPercentage = 0.25f;
        materialChanged = true;
    }
    
    if (materialChanged)
    {
        ComputeMaterialComponents();
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
            
            mLightPosUniform->SetValue(
                                       mLightPos, 0);
            
            mViewPosUniform->SetValue(
                                      IvVector3(0.0f, -10.0f, 0.0f), 0);
            
            IvSetWorldMatrix(transform);
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
        kTNPFormat, verts, NULL, kDefaultUsage);

    // temporary pointers that can be stepped along the arrays
    IvTNPVertex* tempVerts = (IvTNPVertex*)(mSphereVerts->BeginLoadData());

    // A double loop, walking around and down the sphere
    const float phiIncrement = kPI / (steps - 1);
    const float thetaIncrement = kTwoPI / steps;

    unsigned int j;
    for (j = 0; j < steps; j++)
    {
        float theta = thetaIncrement * j;

        float sinTheta, cosTheta;
        IvSinCos(theta, sinTheta, cosTheta);

        float v = j / (float)(steps - 1);

        unsigned int i;
        for (i = 0; i < steps; i++)
        {
            float u = i / (float)(steps - 1);

            float phi = phiIncrement * i - kHalfPI;

            float sinPhi, cosPhi;
            IvSinCos(phi, sinPhi, cosPhi);

            IvVector3 pos(mRadius * cosTheta * cosPhi, mRadius * sinTheta * cosPhi, mRadius * sinPhi);

            tempVerts->position = pos;

            pos.Normalize();

            tempVerts->normal = pos;

            tempVerts->texturecoord = IvVector2(u, v);

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



