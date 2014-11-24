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
    
    IvImage* image = IvImage::CreateFromFile("image.tga");


    IvRenderer::mRenderer->SetShaderProgram(mShader);

    mShader->GetUniform("pointLightIntensity")->SetValue(1.0f, 0);

    mShader->GetUniform("pointLightAttenuation")->SetValue(
        IvVector4(1.0f, 0.0f, 0.0f, 0.0f), 0);

    mShader->GetUniform("materialAmbientColor")->SetValue(
        IvVector4(1.0f, 0.0f, 0.0f, 0.0f), 0);

    mShader->GetUniform("materialDiffuseColor")->SetValue(
        IvVector4(0.0f, 0.0f, 1.0f, 0.0f), 0);

    mShader->GetUniform("materialSpecularColor")->SetValue(
        IvVector4(1.0f, 1.0f, 1.0f, 0.0f), 0);

    mShader->GetUniform("materialSpecularExp")->SetValue(8.0f, 0);

    mLightComponents = IvVector4(0.5f, 0.5f, 0.5f, 0.0f);
    mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);

    mShader->GetUniform("lightColor")->SetValue(IvVector4(1.0f, 1.0f, 1.0f, 0.0f), 0);

    mLightPos = IvVector4(0.0f, -10.0f, 0.0f, 1.0f);

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
        mLightPos += IvVector4(x, y, z, 0.0f);

        lightPosChanged = false;
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('0'))
    {
        mShader->GetUniform("pointLightAttenuation")->SetValue(
            IvVector4(1.0f, 0.0f, 0.0f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('1'))
    {
        mShader->GetUniform("pointLightAttenuation")->SetValue(
            IvVector4(0.0f, 0.25f, 0.0f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('2'))
    {
        mShader->GetUniform("pointLightAttenuation")->SetValue(
            IvVector4(0.0f, 0.0f, 0.0125f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('q'))
    {
        mLightComponents.SetX( mLightComponents.GetX() + dt);
        mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('a'))
    {
        mLightComponents.SetX( mLightComponents.GetX() - dt);
        mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('w'))
    {
        mLightComponents.SetY( mLightComponents.GetY() + dt);
        mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('s'))
    {
        mLightComponents.SetY( mLightComponents.GetY() - dt);
        mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('e'))
    {
        mLightComponents.SetZ( mLightComponents.GetZ() + dt);
        mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('d'))
    {
        mLightComponents.SetZ( mLightComponents.GetZ() - dt);
        mShader->GetUniform("lightAmbDiffSpec")->SetValue(mLightComponents, 0);
    }

    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mLightPos = IvVector4(0.0f, -10.0f, 0.0f, 1.0f);
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

            IvMatrix44 inv = transform;
            inv.AffineInverse();

            mLightPosUniform->SetValue(
                inv * mLightPos, 0);

            mViewPosUniform->SetValue(
                inv * IvVector4(0.0f, -10.0f, 0.0f, 1.0f), 0);

            IvSetWorldMatrix(transform);

            // draw geometry
            IvDrawUnitSphere();
        }
    }

}   // End of Player::Render()
