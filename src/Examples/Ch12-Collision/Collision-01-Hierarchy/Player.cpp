//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This demo shows the power of a simple scene graph, which allows for quick and
// easy creation, management and rendering of hierachical scenes such as the
// articulated submarine used herein.  The demo allows the user to move and rotate
// the submarine, which turns the rudder and dive planes as well.  In addition, 
// the rudder and dive planes can be independently articulated, as well.
//
// This demo also displays the bounding hierarchy for the submarine, showing how 
// the different levels of bounding objects are constructed and relate to one
// another.  The leaf level of bounding capsules is displayed in orange, and
// the internal level of bounding spheres is displayed in yellow.  Each can be 
// toggled on and off.
//
// The key commands are:
//
// j, l - translate sub in x
// i, k - translate sub in y
// z, c - rotate sub around z
// a, d - rotate rudder around z
// w, s - rotate dive planes vertically
// r, f - move periscope up and down
// q  - toggle hierarchy bounds display
// e  - toggle leaf bounds display
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
    // Build the submarine hierarchy, which looks like:
    //                     mBody
    //       /        /      |         \         \
    //  bodyGeom  mTower  mPlanes  mPropeller  mTail
    //            /  |                          |  \
    //   towerGeom   mPeriscope          tailGeom   mRudder

    // Read the sub body geometry from file
    IvFileReader bodyFile("sub_body.txt");
    IvIndexedGeometry* bodyGeom = IvIndexedGeometry::CreateFromStream(bodyFile);

    // Read the sub tower geometry from file
    IvFileReader towerFile("conning.txt");
    IvIndexedGeometry* towerGeom 
        = IvIndexedGeometry::CreateFromStream(towerFile);

    // Read the sub periscope geometry from file
    IvFileReader periFile("periscope.txt");
    IvIndexedGeometry* periGeom 
        = IvIndexedGeometry::CreateFromStream(periFile);

    // Read the sub propeller geometry from file
    IvFileReader propFile("propeller.txt");
    IvIndexedGeometry* propGeom 
        = IvIndexedGeometry::CreateFromStream(propFile);

    // Read the sub dive planes geometry from file
    IvFileReader planesFile("dive_planes.txt");
    IvIndexedGeometry* planesGeom 
        = IvIndexedGeometry::CreateFromStream(planesFile);

    // Read the sub tail geometry from file
    IvFileReader tailFile("tail.txt");
    IvIndexedGeometry* tailGeom 
        = IvIndexedGeometry::CreateFromStream(tailFile);

    // Read the sub rudder geometry from file
    IvFileReader rudderFile("rudder.txt");
    IvIndexedGeometry* rudderGeom 
        = IvIndexedGeometry::CreateFromStream(rudderFile);

    // Create the scene graph from the bottom up
    mPlanes = planesGeom;
    mPlanes->SetLocalTranslate(IvVector3(20.0f, 0.0f, 3.25f));

    mPeriscope = periGeom;
    mPeriscope->SetLocalTranslate(IvVector3(0.0f, 0.0f, 3.0f));

    mPropeller = propGeom;
    mPropeller->SetLocalTranslate(IvVector3(-42.0, 0.0f, 0.0f));

    // create the tower node and attach the children
    mTower = new IvNode(2);
    mTower->SetLocalTranslate(IvVector3(10.0f, 0.0f, 8.0f));

    ((IvNode*)mTower)->SetChild(0, towerGeom);
    ((IvNode*)mTower)->SetChild(1, mPeriscope);

    // create the tail node and attach the children
    mRudder = rudderGeom;
    mRudder->SetLocalTranslate(IvVector3(0.0f, 0.0f, 1.5f));

    mTail = new IvNode(2);
    mTail->SetLocalTranslate(IvVector3(-37.5f, 0.0f, 0.0f));
    ((IvNode*)mTail)->SetChild(0, tailGeom);
    ((IvNode*)mTail)->SetChild(1, mRudder);

    // create the body node and attach the children
    mBody = new IvNode(5);
    ((IvNode*)mBody)->SetChild(0, bodyGeom);
    ((IvNode*)mBody)->SetChild(1, mTower);
    ((IvNode*)mBody)->SetChild(2, mPlanes);
    ((IvNode*)mBody)->SetChild(3, mPropeller);
    ((IvNode*)mBody)->SetChild(4, mTail);

    // Scale the entire scene down to fit in view
    mBody->SetLocalScale(0.15f);

    // Update the transforms and bounds
    mBody->UpdateWorldTransform();

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    // no need to delete the child nodes - root of scene (sub) will delete them
    delete mBody;

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

    // set up submarine scale
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
        mBody->SetLocalScale( mBody->GetLocalScale() * s);
    }
    
    // set up submarine rotate
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
        mBody->SetLocalRotate(mBody->GetLocalRotate() * rotate);
    }

    // set up planes rotate
    {
        float r = 0;
        if (IvGame::mGame->mEventHandler->IsKeyDown('w'))
        {
            r += kPI*0.25f*dt;
            update = true;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('s'))
        {
            r -= kPI*0.25f*dt;
            update = true;
        }
        IvMatrix33 rotate;
        rotate.RotationY(r);
        mPlanes->SetLocalRotate(mPlanes->GetLocalRotate() * rotate);
    }
    
    // set up rudder rotate
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
        mRudder->SetLocalRotate(mRudder->GetLocalRotate() * rotate);
    }
    
    // set up periscope translate
    {
        IvVector3 xlate;
        xlate.Zero();
        if (IvGame::mGame->mEventHandler->IsKeyDown('r'))
        {
            xlate += dt*IvVector3::zAxis;
            update = true;
        }
        if (IvGame::mGame->mEventHandler->IsKeyDown('f'))
        {
            xlate -= dt*IvVector3::zAxis;
            update = true;
        }
        mPeriscope->SetLocalTranslate( mPeriscope->GetLocalTranslate() + xlate );
    }
    
    // set up submarine translation
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
    mBody->SetLocalTranslate(mBody->GetLocalTranslate() + xlate);
    
    // clear transforms
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        IvMatrix33 reset;
        reset.Identity();

        mBody->SetLocalRotate(reset);
        mBody->SetLocalTranslate(IvVector3::origin);
        mBody->SetLocalScale(0.15f);

        mPlanes->SetLocalRotate(reset);
        mRudder->SetLocalRotate(reset);
        mPeriscope->SetLocalTranslate(IvVector3::origin);
        update = true;
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('q'))
    {
        IvSpatial::gDisplayHierarchyBounds = !IvSpatial::gDisplayHierarchyBounds;
        IvGame::mGame->mEventHandler->KeyUp('q');
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('e'))
    {
        IvSpatial::gDisplayLeafBounds = !IvSpatial::gDisplayLeafBounds;
        IvGame::mGame->mEventHandler->KeyUp('e');
    }

    // For ease of use, just update the entire scene if any transforms have
    // changed.  We do avoid this if no transforms have changed, though
    if (update)
        mBody->UpdateWorldTransform();

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
    mBody->Render();

}   // End of Player::Render()


