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

#include <IvEventHandler.h>
#include <IvFileReader.h>
#include <IvRendererHelp.h>
#include <IvImage.h>
#include <IvIndexedGeometry.h>
#include <IvMath.h>
#include <IvNode.h>
#include <IvMatrix44.h>
#include <IvSpatial.h>

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
    // Build the tank hierarchy, which looks like:
    //               mTank (IvNode)
    //              /              \
    // tankGeom (IvIndexedGeom)     mTurret (IvNode)
    //                             /                \
    //        turretGeom (IvIndexedGeom)     mBarrel = barrelGeom (IvIndexedGeom)

    // Read the tank body geometry from file
    IvFileReader tankFile("Tank.txt");
    IvIndexedGeometry* tankGeom = IvIndexedGeometry::CreateFromStream(tankFile);

    // Read the tank turret geometry from file
    IvFileReader turretFile("Turret.txt");
    IvIndexedGeometry* turretGeom 
        = IvIndexedGeometry::CreateFromStream(turretFile);

    // Read the tank barrel geometry from file
    IvFileReader barrelFile("Barrel.txt");
    IvIndexedGeometry* barrelGeom 
        = IvIndexedGeometry::CreateFromStream(barrelFile);

    // Create the scene graph from the bottom up
    mBarrel = barrelGeom;
    mBarrel->SetLocalTranslate(IvVector3(4.5f, 0.0f, 1.0f));

    // create the turret node and attach the children
    mTurret = new IvNode(2);

    mTurret->SetLocalTranslate(IvVector3(0.75f, 0.0f, 3.55f));
    ((IvNode*)mTurret)->SetChild(0, turretGeom);
    ((IvNode*)mTurret)->SetChild(1, mBarrel);

    // create the tank node and attach the children
    mTank = new IvNode(2);

    ((IvNode*)mTank)->SetChild(0, tankGeom);
    ((IvNode*)mTank)->SetChild(1, mTurret);

    // Scale the entire scene down to fit in view
    mTank->SetLocalScale(0.25f);

    // Update the transforms and bounds
    mTank->UpdateWorldTransform();

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    // no need to delete the child nodes - root of scene (tank) will delete them
    delete mTank;

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
        mTank->SetLocalScale( mTank->GetLocalScale() * s);
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

        IvMatrix33 rotate;
        rotate.RotationZ(r);
        mTank->SetLocalRotate(mTank->GetLocalRotate() * rotate);
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
        IvMatrix33 rotate;
        rotate.RotationZ(r);
        mTurret->SetLocalRotate(mTurret->GetLocalRotate() * rotate);
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
        IvMatrix33 rotate;
        rotate.RotationY(r);
        mBarrel->SetLocalRotate(mBarrel->GetLocalRotate() * rotate);
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
    mTank->SetLocalTranslate(mTank->GetLocalTranslate() + xlate);
    
    // clear transforms
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        IvMatrix33 reset;
        reset.Identity();

        mTank->SetLocalRotate(reset);
        mTank->SetLocalTranslate(IvVector3::origin);
        mTank->SetLocalScale(0.25f);

        mTurret->SetLocalRotate(reset);
        mBarrel->SetLocalRotate(reset);

        update = true;
    }

    // For ease of use, just update the entire scene if any transforms have
    // changed.  We do avoid this if no transforms have changed, though
    if (update)
        mTank->UpdateWorldTransform();

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
    mTank->Render();

}   // End of Player::Render()


