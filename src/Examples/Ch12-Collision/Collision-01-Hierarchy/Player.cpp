//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player object
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
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

#include <IvAssert.h>
#include <IvEventHandler.h>
#include <IvFileReader.h>
#include <IvRendererHelp.h>
#include <IvImage.h>
#include <IvMath.h>
#include <IvMatrix44.h>

#include "Player.h"
#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

const int kBodyIndex = 0;
const int kTowerIndex = 1;
const int kPlanesIndex = 2;
const int kPropellerIndex = 3;
const int kTailIndex = 4;
const int kPeriscopeIndex = 5;
const int kRudderIndex = 6;
const int kNumNodes = 7;

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
    //                          Body
    //                /      /         \         \
    //             Tower   Planes   Propeller   Tail
    //               |                            | 
    //           Periscope                      Rudder

    mHierarchy.AllocNodes(kNumNodes);

    // Read the sub body geometry from file
    bool result;
    IvFileReader bodyFile("sub_body.txt");
    result = mHierarchy.AddNode(kBodyIndex, kBodyIndex, bodyFile, 
                                IvVector3::origin, IvQuat(IvVector3::zAxis, 0.5f*kPI), 0.15f);
    ASSERT(result);

    // Read the sub tower geometry from file
    IvFileReader towerFile("conning.txt");
    result = mHierarchy.AddNode(kTowerIndex, kBodyIndex, towerFile, 
                                IvVector3(10.0f, 0.0f, 8.0f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the sub periscope geometry from file
    IvFileReader periFile("periscope.txt");
    result = mHierarchy.AddNode(kPeriscopeIndex, kTowerIndex, periFile, 
                                IvVector3(0.0f, 0.0f, 3.0f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the sub propeller geometry from file
    IvFileReader propFile("propeller.txt");
    result = mHierarchy.AddNode(kPropellerIndex, kBodyIndex, propFile, 
                                IvVector3(-42.0, 0.0f, 0.0f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the sub dive planes geometry from file
    IvFileReader planesFile("dive_planes.txt");
    result = mHierarchy.AddNode(kPlanesIndex, kBodyIndex, planesFile, 
                                IvVector3(20.0f, 0.0f, 3.25f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the sub tail geometry from file
    IvFileReader tailFile("tail.txt");
    result = mHierarchy.AddNode(kTailIndex, kBodyIndex, tailFile, 
                                IvVector3(-37.5f, 0.0f, 0.0f), IvQuat(), 1.0f);
    ASSERT(result);

    // Read the sub rudder geometry from file
    IvFileReader rudderFile("rudder.txt");
    result = mHierarchy.AddNode(kRudderIndex, kTailIndex, rudderFile, 
                                IvVector3(0.0f, 0.0f, 1.5f), IvQuat(), 1.0f);
    ASSERT(result);

    // Update the transforms and bounds
    mHierarchy.UpdateWorldTransforms();

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
        mHierarchy.SetLocalScale(mHierarchy.GetLocalScale(kBodyIndex) * s, kBodyIndex);
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

        IvQuat rotate(IvVector3::zAxis, r);
        mHierarchy.SetLocalRotate(mHierarchy.GetLocalRotate(kBodyIndex) * rotate, kBodyIndex);
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
        IvQuat rotate(IvVector3::yAxis, r);;
        mHierarchy.SetLocalRotate(mHierarchy.GetLocalRotate(kPlanesIndex) * rotate, kPlanesIndex);
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
        IvQuat rotate(IvVector3::zAxis, r);
        mHierarchy.SetLocalRotate(mHierarchy.GetLocalRotate(kRudderIndex) * rotate, kRudderIndex);
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
        mHierarchy.SetLocalTranslate(mHierarchy.GetLocalTranslate(kPeriscopeIndex) + xlate, kPeriscopeIndex);
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
    mHierarchy.SetLocalTranslate(mHierarchy.GetLocalTranslate(kBodyIndex) + xlate, kBodyIndex);
    
    // clear transforms
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        IvQuat reset;
        reset.Identity();

        mHierarchy.SetLocalRotate(IvQuat(IvVector3::zAxis, 0.5f*kPI), kBodyIndex);
        mHierarchy.SetLocalTranslate(IvVector3::origin, kBodyIndex);
        mHierarchy.SetLocalScale(0.15f, kBodyIndex);

        mHierarchy.SetLocalRotate(reset, kPlanesIndex);
        mHierarchy.SetLocalRotate(reset, kRudderIndex);
        mHierarchy.SetLocalTranslate(IvVector3::origin, kPeriscopeIndex);
        update = true;
    }

    if (IvGame::mGame->mEventHandler->IsKeyDown('q'))
    {
        IvHierarchy::gDisplayHierarchyBounds = !IvHierarchy::gDisplayHierarchyBounds;
        IvGame::mGame->mEventHandler->KeyUp('q');
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('e'))
    {
        IvHierarchy::gDisplayLeafBounds = !IvHierarchy::gDisplayLeafBounds;
        IvGame::mGame->mEventHandler->KeyUp('e');
    }

    // For ease of use, just update the entire scene if any transforms have
    // changed.  We do avoid this if no transforms have changed, though
    if (update)
    {
        mHierarchy.UpdateWorldTransforms();
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
    mHierarchy.Render();

}   // End of Player::Render()


