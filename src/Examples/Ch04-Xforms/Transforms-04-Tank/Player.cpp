//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This example demonstrates one way to build a transform hierarchy.  A tank 
// model is constructed of three pieces: body, turret and barrel.  When rendered
// each step in the hierarchy passes its world transform to its children.  This
// is concatentated with the child's local transform to get a final world transform
// for the child.
//
// The key commands are:
//
// I,J,K,L: translate tank in the XY plane
// U,O: rotate tank around Z axis
// ;,P: scale tank down, up
// A,D: rotate turret left/right
// W,S: raise barrel up/down
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvFileReader.h>
#include <IvRendererHelp.h>
#include <IvMath.h>
#include <IvMatrix44.h>
#include <IvReader.h>
#include <IvResourceManager.h>
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>

#include "Player.h"
#include "Game.h"
#include "Turret.h"

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
Player::Player() :
    mVertices( 0 ),
    mIndices( 0 )
{
    mRotate.Identity();
    mScale = 0.25f;
    mTranslate.Zero();
    
    mTurret = new Turret();

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    delete mTurret;
    mTurret = 0;

    CleanData();

}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::ReadData()
//-------------------------------------------------------------------------------
// Read in geometry data
//-------------------------------------------------------------------------------
bool
Player::ReadData()
{
    // read in geometric data
    IvFileReader in("Tank.txt");
    if (!in)
        return false;

    unsigned int numVerts;
    
    // get number of vertices
    in >> numVerts;
    if ( !in.good() )
        return false;

    mVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCNPFormat, numVerts,
                                                                                NULL, kDefaultUsage);
    IvCNPVertex* dataPtr = (IvCNPVertex*) mVertices->BeginLoadData();

    // read positions
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float x, y, z;
        in >> x >> y >> z;
        dataPtr[i].position.Set(x,y,z);
        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }

    // read normals
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float x, y, z;
        in >> x >> y >> z;
        dataPtr[i].normal.Set(x,y,z);
        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }

    // read colors
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float r, g, b;
        in >> r >> g >> b;
        dataPtr[i].color.mRed = UChar8(r*255);
        dataPtr[i].color.mGreen = UChar8(g*255);
        dataPtr[i].color.mBlue = UChar8(b*255);
        dataPtr[i].color.mAlpha = 255;

        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }

    if (!mVertices->EndLoadData())
    {
        CleanData();
        return false;
    }

    // get number of indices
    UInt32 numTankBodyIndices;
    in >> numTankBodyIndices;
    if ( !in.good() )
        return false;

    // read indices
    mIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numTankBodyIndices,
                                                                              NULL, kDefaultUsage);
    UInt32* indexPtr = static_cast<UInt32*>(mIndices->BeginLoadData());
    for ( UInt32 i = 0; i < numTankBodyIndices; ++i )
    {
        in >> indexPtr[i];
        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }
    if (!mIndices->EndLoadData())
    {
        CleanData();
        return false;
    }

    // read in turret
    if ( !mTurret || !mTurret->ReadData() )
    {
        CleanData();
        return false;
    }

    return true;

}   // End of Player::ReadData()


//-------------------------------------------------------------------------------
// @ Player::CleanData()
//-------------------------------------------------------------------------------
// Clean geometry data
//-------------------------------------------------------------------------------
void 
Player::CleanData()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
    mVertices = 0;
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
    mIndices = 0;

}   // End of Player::CleanData()


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    float s = 1.0f;
    float r = 0.0f;
    float x = 0.0f, y = 0.0f, z = 0.0f; 
    IvMatrix33 rotate;
    IvVector3 xlate;
    IvMatrix44 transform;
    
    mTurret->Update( dt );
    
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
    rotate.RotationZ(r);
    mRotate = rotate*mRotate;
    
    // set up translation
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        x -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        x += 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        y -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        y += 3.0f*dt;
    }
    xlate.Set( x, y, z );
    mTranslate += xlate;
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
        mScale = 0.25f;
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
    mTurret->Render( mScale, mRotate, mTranslate );

    // build 4x4 matrix
    IvMatrix44 transform( mRotate );
    transform(0,0) *= mScale;
    transform(1,0) *= mScale;
    transform(2,0) *= mScale;
    transform(0,1) *= mScale;
    transform(1,1) *= mScale;
    transform(2,1) *= mScale;
    transform(0,2) *= mScale;
    transform(1,2) *= mScale;
    transform(2,2) *= mScale;
    transform(0,3) = mTranslate.x;
    transform(1,3) = mTranslate.y;
    transform(2,3) = mTranslate.z;
    
    IvSetWorldMatrix( transform );
    IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices, mIndices);
}


