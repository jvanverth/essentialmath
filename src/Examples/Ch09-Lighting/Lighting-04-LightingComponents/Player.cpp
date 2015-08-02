//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
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

#include <IvImage.h>


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

    mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
        IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile(
        "componentsShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "componentsShader"));
    
    IvRenderer::mRenderer->SetShaderProgram(mShader);

    mShader->GetUniform("pointLightIntensity")->SetValue(IvVector3(20.f, 20.f, 0.0f), 0);

    mShader->GetUniform("ambientLightColor")->SetValue(IvVector3(0.0f, 0.0f, 0.2f), 0);
    
    mLightPos = IvVector3(0.0f, -10.0f, 0.0f);
    
    mSpecularPercentage = 0.25f;
    mAmbientFactor = 0.1f;
    mEmissiveFactor = 0.0f;
    
    ComputeMaterialComponents();

    mLightPosUniform = mShader->GetUniform("pointLightPosition");
    mViewPosUniform = mShader->GetUniform("viewPosition");

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mShader);
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
    
    // clear transform
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
    transform.Scaling(IvVector3(mRadius, mRadius, mRadius));

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
            IvDrawUnitSphere();
        }
    }

}   // End of Player::Render()
