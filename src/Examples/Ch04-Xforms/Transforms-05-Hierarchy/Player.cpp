//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
// This demo shows the power of a simple scene graph, which allows for quick and
// easy creation, management and rendering of hierachical scenes such as the
// articulated tank used herein.  The demo allows the user to move and rotate
// the tank, which turns the turret and barrel as well.  In addition, the turret
// and barrel can be independently articulated, as well
//
// The key commands are:
//
// j, l - translate tank in x
// i, k - translate tank in y
// z, c - rotate tank around z
// a, d - rotate turret around z
// w, s - rotate barrel vertically
// space - reset all transforms
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvAssert.h>
#include <IvEventHandler.h>
#include <IvFileReader.h>
#include <IvRendererHelp.h>
#include <IvImage.h>
#include <IvIndexedGeometry.h>
#include <IvMath.h>
#include <IvMatrix44.h>

#include "Player.h"
#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

const int kTankIndex = 0;
const int kTurretIndex = 1;
const int kBarrelIndex = 2;

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
    // Build the tank hierarchy, which looks like:
    //               Tank 
    //                |           
    //              Turret
    //                |             
    //              Barrel

    bool result;
    result = mTank.AllocNodes(3);
    ASSERT(result);

    // Read the tank body geometry from file
    IvFileReader tankFile("Tank.txt");
    result = mTank.AddNode(kTankIndex, kTankIndex, tankFile,
                           IvVector3::origin, IvQuat(), 0.2f);
    ASSERT(result);

    // Read the tank turret geometry from file
    IvFileReader turretFile("Turret.txt");
    result = mTank.AddNode(kTurretIndex, kTankIndex, turretFile,
                           IvVector3(0.75f, 0.0f, 3.55f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the tank barrel geometry from file
    IvFileReader barrelFile("Barrel.txt");
    result = mTank.AddNode(kBarrelIndex, kTurretIndex, barrelFile,
                           IvVector3(4.5f, 0.0f, 1.0f), IvQuat(), 1.0f);
    ASSERT(result);

    // Update the transforms and bounds
    mTank.UpdateWorldTransforms();

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
    bool update = false;

    // set up tank scale
    {
        float s = 1.0f;
        if (IvGame::mGame->mEventHandler->IsKeyDown(';'))
        {
            s -= 0.25f*dt;
            update = true;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('p'))
        {
            s += 0.25f*dt;
            update = true;
        }
        mTank.SetLocalScale(mTank.GetLocalScale(kTankIndex) * s, kTankIndex);
    }
    
    // set up tank rotate
    {
        float r = 0.0f;
        if (IvGame::mGame->mEventHandler->IsKeyDown('u'))
        {
            r += kPI*0.25f*dt;
            update = true;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('o'))
        {
            r -= kPI*0.25f*dt;
            update = true;
        }

        IvQuat rotate(IvVector3::zAxis, r);
        mTank.SetLocalRotate(mTank.GetLocalRotate(kTankIndex) * rotate, kTankIndex);
    }

    // set up turret rotate
    {
        float r = 0;
        if (IvGame::mGame->mEventHandler->IsKeyDown('a'))
        {
            r += kPI*0.25f*dt;
            update = true;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('d'))
        {
            r -= kPI*0.25f*dt;
            update = true;
        }
        IvQuat rotate(IvVector3::zAxis, r);
        mTank.SetLocalRotate(mTank.GetLocalRotate(kTurretIndex) * rotate, kTurretIndex);
    }
    
    // set up barrel rotate
    {
        float r = 0;
        if (IvGame::mGame->mEventHandler->IsKeyDown('s'))
        {
            r += kPI*0.25f*dt;
            update = true;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('w'))
        {
            r -= kPI*0.25f*dt;
            update = true;
        }
        IvQuat rotate(IvVector3::yAxis, r);
        mTank.SetLocalRotate(mTank.GetLocalRotate(kBarrelIndex) * rotate, kBarrelIndex);
    }
    
    // set up tank translation
    IvVector3 xlate;
    xlate.Zero();
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        xlate -= 3.0f*dt*IvVector3::xAxis;
        update = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        xlate += 3.0f*dt*IvVector3::xAxis;
        update = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        xlate -= 3.0f*dt*IvVector3::yAxis;
        update = true;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        xlate += 3.0f*dt*IvVector3::yAxis;
        update = true;
    }
    mTank.SetLocalTranslate(mTank.GetLocalTranslate(kTankIndex) + xlate, kTankIndex);
    
    // clear transforms
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        IvQuat reset;
        reset.Identity();

        mTank.SetLocalRotate(reset, kTankIndex);
        mTank.SetLocalTranslate(IvVector3::origin, kTankIndex);
        mTank.SetLocalScale(0.25f, kTankIndex);

        mTank.SetLocalRotate(reset, kTurretIndex);
        mTank.SetLocalRotate(reset, kBarrelIndex);

        update = true;
    }

    // For ease of use, just update the entire scene if any transforms have
    // changed.  We do avoid this if no transforms have changed, though
    if (update)
    {
        mTank.UpdateWorldTransforms();
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
    // Just render the root - the scene graph handles the rest for us.
    mTank.Render();

}   // End of Player::Render()


