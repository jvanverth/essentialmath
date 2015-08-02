//===============================================================================
// @ Game.cpp
// ------------------------------------------------------------------------------
// Game core routines
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// This code example shows how to invert the OpenGL projection matrix to do 
// picking.  Game::GetPickRay() computes the pick ray based on a standard
// gluPerspective() matrix and the inverse view matrix.
//
// The key commands are:
//
// Click with left (or only) mouse button to move teapot around
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <math.h>

#include <IvClock.h>
#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvMatrix33.h>
#include <IvMatrix44.h>
#include <IvRendererHelp.h>
#include <IvVector4.h>

#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvGame::Create()
//-------------------------------------------------------------------------------
// Static constructor
//-------------------------------------------------------------------------------
bool
IvGame::Create()
{
    IvGame::mGame = new Game();
    return ( IvGame::mGame != 0 );

}   // End of IvGame::Create()


//-------------------------------------------------------------------------------
// @ Game::Game()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Game::Game() : IvGame(),
    mClickPosition( 0.0f, 0.0f, 0.0f )
{

}   // End of Game::Game()


//-------------------------------------------------------------------------------
// @ Game::~Game()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Game::~Game()
{
}   // End of Game::~Game()


//-------------------------------------------------------------------------------
// @ Game::PostRendererInitialize()
//-------------------------------------------------------------------------------
// Set up internal subsystems
//-------------------------------------------------------------------------------
bool 
Game::PostRendererInitialize()
{
    // Set up base class 
    if ( !IvGame::PostRendererInitialize() )
        return false;

    ::IvSetDefaultLighting();
    
    return true;
}   // End of Game::PostRendererInitialize()


//-------------------------------------------------------------------------------
// @ Game::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Game::UpdateObjects( float /*dt*/ )
{
    // handle picking
    unsigned int h, v;
    if (mEventHandler->IsMouseDown( h, v ))
    {
        // get the pick ray
        IvVector3 ray = GetPickRay( (float) h, (float) v, IvRenderer::mRenderer->GetFOV(),
                                    (float) IvRenderer::mRenderer->GetWidth(), (float) IvRenderer::mRenderer->GetHeight() );

        // compute intersection with z=0 plane
        float t = -mEyePoint.z/ray.z;
        mClickPosition = mEyePoint + t*ray;

        mEventHandler->MouseUp();
    }
        
}   // End of Game::Update()


//-------------------------------------------------------------------------------
// @ Game::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Game::Render()                                  // Here's Where We Do All The Drawing
{       
    // look from (-10, 2, 10) at the origin
    LookAt( IvVector3(-10.0f, 2.0f, 10.0f), IvVector3::origin, IvVector3::zAxis );  
    
    // draw axes
    IvDrawAxes();

    // draw a floor
    IvDrawFloor();
    
    // draw the main object
    IvMatrix44 worldTransform;
    worldTransform.Translation( mClickPosition );
    IvSetWorldMatrix( worldTransform );
    IvDrawTeapot();
}


//-------------------------------------------------------------------------------
// @ Game::LookAt()
//-------------------------------------------------------------------------------
// Basic camera lookAt code
//-------------------------------------------------------------------------------
void 
Game::LookAt( const IvVector3& eye, const IvVector3& lookAt, const IvVector3& up )                              
{
    mEyePoint = eye;

    // compute view vectors
    IvVector3 view = lookAt - eye;
    IvVector3 right;
    IvVector3 viewUp;
    view.Normalize();
    right =  view.Cross( up );
    right.Normalize();
    viewUp =  right.Cross( view );
    viewUp.Normalize();

    // now set up matrices
    // base rotation matrix
    IvMatrix33 rotate;
    if ( IvRenderer::mRenderer->GetAPI() == kOpenGL )
    {
        rotate.SetColumns( right, viewUp, -view );
    }
    else
    {
        rotate.SetColumns( right, viewUp, view );
    }

    // view->world transform
    // set rotation
    mViewToWorldMatrix.Rotation(rotate);
    // set translation (eye position)
    mViewToWorldMatrix(0,3) = eye.x;
    mViewToWorldMatrix(1,3) = eye.y;
    mViewToWorldMatrix(2,3) = eye.z;

    // world->view transform
    // set rotation
    rotate.Transpose();
    mWorldToViewMatrix.Rotation(rotate);
    // set translation (rotate into view space)
    IvVector3 invEye = -(rotate*eye);
    mWorldToViewMatrix(0,3) = invEye.x;
    mWorldToViewMatrix(1,3) = invEye.y;
    mWorldToViewMatrix(2,3) = invEye.z;

    // send to OpenGL
    IvSetViewMatrix( mWorldToViewMatrix );
}


//-------------------------------------------------------------------------------
// @ Game::GetPickRay()
//-------------------------------------------------------------------------------
// Get pick ray from screen position
//-------------------------------------------------------------------------------
IvVector3
Game::GetPickRay( float sx, float sy, float fov, float width, float height )
{
    float d = 1.0f/IvTan(fov*kPI/360.0f);
    float aspect = width/height;
    IvVector3 viewPoint( 2.0f*aspect*sx/width - aspect, -2.0f*sy/height + 1.0f, 
                         IvRenderer::mRenderer->GetAPI() == kOpenGL ? -d : d );

    viewPoint = mViewToWorldMatrix.TransformPoint( viewPoint );

    return viewPoint-mEyePoint;
}


