//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// Player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This demo shows the effect of different integration methods on a simple 
// orbiting object.  You can toggle between seven different integration methods:
// Euler's method, midpoint method, Runge-Kutta 4, velocity Verlet, a naive 
// implicit method, a simple predictor-corrector, and symplectic Euler.  
// Note how the Euler becomes more unstable, and the midpoint as well but less so.  
// The implicit method, on the other hand, spirals inward.  The symplectic method 
// is the most tolerant of all of them.
//
// The key commands are:
//
// M: switch integration method
// SPACE: reset
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvResourceManager.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>
#include <IvEventHandler.h>
#include <IvMatrix44.h>
#include <IvDebugger.h>
#include <IvMath.h>

#include <vector>

#include "Player.h"
#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

float radius = 1.0f;
float duration = 4.0f;
float velocity = 2.0f*kPI*radius/duration;
float acceleration = velocity*velocity/radius;

IvVector3 oldPosition;
bool firstStep = true;

std::vector<IvVector3> positions;

const char* modeStrings[] =
{
    "Euler", "Midpoint", 
    "RK4", "NaiveImplicit", "PredictorCorrector", 
    "Symplectic", "Verlet"
};

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
    mTranslate = radius*IvVector3::zAxis;
    mVelocity = -velocity*IvVector3::yAxis;
    mMass = 10.0f;

    mMode = kEuler;
    SetWindowTitle( modeStrings[mMode] );

    positions.push_back( mTranslate );

    mFrameCounter = 0;
    mOrbit = 0;
    mPath = 0;

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mPath);
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mOrbit);
}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    // we'll use a fixed integration time to get fixed results at all frame rates
    dt = .01f;

    if (IvGame::mGame->mEventHandler->IsKeyPressed('m'))
    {
        mMode = (Mode)((mMode+1)%kMaxMode);
        SetWindowTitle( modeStrings[mMode] );

        mTranslate = radius*IvVector3::zAxis;
        mVelocity = -velocity*IvVector3::yAxis;

        mFrameCounter = 0;
        positions.clear();
    }

    // reset simulation
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mTranslate = radius*IvVector3::zAxis;
        mVelocity = -velocity*IvVector3::yAxis;
        positions.clear();
        mFrameCounter = 0;
    }

    // integrate
    IvVector3 accel = CurrentForce( mTranslate, mVelocity )/mMass;
    // clear small values
    accel.Clean();          

    if ( mMode == kEuler )
    {
        // Euler's method
        // compute new position, velocity
        mTranslate += mVelocity*dt;
        mVelocity += accel*dt;
        // clear small values
        mVelocity.Clean();  
    }
    else if ( mMode == kMidpoint )
    {
        // Midpoint method
        // compute new position, velocity
        float h2 = 0.5f*dt;
        IvVector3 midPosition = mTranslate + h2*mVelocity;
        IvVector3 midVelocity = mVelocity + h2*accel;
        // clear small values
        midVelocity.Clean();        

        accel = CurrentForce( midPosition, midVelocity ) / mMass;
        // clear small values
        accel.Clean();          

        // compute new position, velocity
        mTranslate += dt*mVelocity;
        mVelocity += dt*accel;
        // clear small values
        mVelocity.Clean();      
    }
    else if ( mMode == kRK4 )
    {
        // Runge-Kutta 4 method
        // compute steps
        IvVector3 xk1 = dt*mVelocity;
        IvVector3 vk1 = dt*accel;

        IvVector3 midVelocity = mVelocity + 0.5f*vk1;
        IvVector3 xk2 = dt*(midVelocity);
        IvVector3 vk2 = dt*(CurrentForce( mTranslate + 0.5f*xk1, midVelocity ) / mMass);

        midVelocity = mVelocity + 0.5f*vk2;
        IvVector3 xk3 = dt*(midVelocity);
        IvVector3 vk3 = dt*(CurrentForce( mTranslate + 0.5f*xk2, midVelocity ) / mMass);

        midVelocity = mVelocity + vk3;
        IvVector3 xk4 = dt*(midVelocity);
        IvVector3 vk4 = dt*(CurrentForce( mTranslate + xk3, midVelocity ) / mMass);

        // compute new position, velocity
        mTranslate += (xk1 + 2.0f*xk2 + 2.0f*xk3 + xk4)/6.0f;
        mVelocity += (vk1 + 2.0f*vk2 + 2.0f*vk3 + vk4)/6.0f;
        // clear small values
        mVelocity.Clean();      
    }
    else if ( mMode == kVerlet )
    {
        // Verlet method
        // compute new position
        IvVector3 halfStepVelocity = mVelocity + 0.5f*dt*accel;
        mTranslate += dt*halfStepVelocity;

        // recompute acceleration
        // we really shouldn't use velocity in computing our forces, 
        // but it's pretty close
        accel = CurrentForce( mTranslate, halfStepVelocity ) / mMass;
        // clear small values
        accel.Clean();          

        // compute new velocity
        mVelocity = halfStepVelocity + 0.5f*dt*accel;
        // clear small values
        mVelocity.Clean();      
    }
    else if ( mMode == kNaiveImplicit )
    {
        // naive predictor-corrector
        // compute new position, velocity
        IvVector3 midTranslate = mTranslate + mVelocity*dt;
        IvVector3 midVelocity = mVelocity + accel*dt;
        // clear small values
        midVelocity.Clean();

        IvVector3 midAccel = CurrentForce( midTranslate, midVelocity )/mMass;
        // clear small values
        midAccel.Clean(); 

        mTranslate += dt*(midVelocity);
        mVelocity += dt*(midAccel);
    }
    else if ( mMode == kPredictorCorrector )
    {
        // first order predictor-corrector
        // compute new position, velocity
        IvVector3 midTranslate = mTranslate + mVelocity*dt;
        IvVector3 midVelocity = mVelocity + accel*dt;
        // clear small values
        midVelocity.Clean();

        IvVector3 midAccel = CurrentForce( midTranslate, midVelocity )/mMass;
        // clear small values
        midAccel.Clean(); 

        mTranslate += 0.5f*dt*(midVelocity + mVelocity);
        mVelocity += 0.5f*dt*(midAccel + accel);
    }
    else 
    {
        // symplectic Euler
        mVelocity += accel*dt;
        // clear small values
        mVelocity.Clean();  
        // compute new position, velocity
        mTranslate += mVelocity*dt;
    }

    if (mFrameCounter == 0)
    {
        positions.push_back( mTranslate );
    }
    mFrameCounter = (mFrameCounter + 1) % 15;

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::CurrentForce()
//-------------------------------------------------------------------------------
// Compute force, given forces constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
Player::CurrentForce( const IvVector3& position, const IvVector3& velocity )                                
{   
    IvVector3 totalForce = -position;

    float localRadius = position.Length();
    float localVelocity = 2.0f*kPI*localRadius/duration;
    float localAcceleration = localVelocity*localVelocity/localRadius;

    totalForce.Normalize();
    totalForce *= localAcceleration*mMass;

    return totalForce;

}   // End of Player::CurrentForce()


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
    transform.Translation( mTranslate );
    
    IvSetWorldMatrix(transform);
    IvDrawSphere(0.25f, kCyan );

    // draw expected orbit
    if ( !mOrbit )
    {
        mOrbit = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 33,
                                                                                 nullptr, kDefaultUsage);
        IvCPVertex* dataPtr = (IvCPVertex*) mOrbit->BeginLoadData();
        for (UInt32 i = 0; i <= 32; ++i)
        {
            float theta = (float)i*2.0f*kPI/32.0f;
            dataPtr[i].position.Set( 0.0f, radius*cosf(theta), radius*sinf(theta) );
            dataPtr[i].color = kWhite;
        }
        (void) mOrbit->EndLoadData();
    }
    IvSetWorldIdentity();
    IvRenderer::mRenderer->Draw( kLineStripPrim, mOrbit, 33 );

    // draw the actual path
    if ( !mPath )
    {
        mPath = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 1280,
                                                                                nullptr, kDynamicUsage);
    }

    IvCPVertex* dataPtr = (IvCPVertex*) mPath->BeginLoadData();
    UInt32 numVerts = positions.size() < 1280 ? positions.size() : 1280; 
    for (size_t i = 0; i < numVerts; ++i)
    {
        dataPtr[i].position = positions[i];
        dataPtr[i].color = kYellow;
    }
    (void) mPath->EndLoadData();

    IvSetWorldIdentity();
    IvRenderer::mRenderer->Draw( kLineStripPrim, mPath, numVerts );
}

