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
        "spotShader"),
        IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile(
        "spotShader"));

    IvRenderer::mRenderer->SetShaderProgram(mShader);

    mShader->GetUniform("spotLightIntensity")->SetValue(IvVector3(1.0f, 1.0f, 1.0f), 0);

    mShader->GetUniform("spotLightAttenuation")->SetValue(
        IvVector3(1.0f, 0.0f, 0.0f), 0);

    mShader->GetUniform("spotLightCosOuterInner")->SetValue(IvVector3(0.75f, 0.8f, 0.0f), 0);

    mLightPos = IvVector3(0.0f, -10.0f, 0.0f);
    mLightDir = IvVector3(0.0f, 1.0f, 0.0f);

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
        mShader->GetUniform("spotLightAttenuation")->SetValue(
            IvVector4(1.0f, 0.0f, 0.0f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('1'))
    {
        mShader->GetUniform("spotLightAttenuation")->SetValue(
            IvVector4(0.0f, 0.25f, 0.0f, 0.0f), 0);
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('2'))
    {
        mShader->GetUniform("spotLightAttenuation")->SetValue(
            IvVector4(0.0f, 0.0f, 0.0125f, 0.0f), 0);
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
    transform.Scaling(IvVector3(mRadius, mRadius, mRadius));

    int i,j;
    for (j = -1; j <= 1; j++)
    {
        for (i = -1; i <= 1; i++)
        {
            transform(0, 3) = 5.0f * i;
            transform(2, 3) = 5.0f * j;

            mShader->GetUniform("spotLightPosition")->SetValue(
                mLightPos, 0);
            mShader->GetUniform("spotLightDir")->SetValue(
                mLightDir, 0);

            ::IvSetWorldMatrix(transform);
            mShader->GetUniform("modelMatrix")->SetValue(transform, 0);

            // draw geometry
            IvDrawUnitSphere();
        }
    }
    
}   // End of Player::Render()
