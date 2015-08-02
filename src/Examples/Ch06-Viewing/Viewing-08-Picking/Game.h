//===============================================================================
// @ Game.h
// ------------------------------------------------------------------------------
// Game core
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//
//
//===============================================================================

#ifndef __GameDefs__
#define __GameDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvGame.h>
#include <IvMatrix44.h>
#include <IvTypes.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class Game : public IvGame
{
public:
    Game();
    ~Game();
    bool PostRendererInitialize();

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();

    void LookAt( const IvVector3& eye, const IvVector3& lookAt, const IvVector3& up );  
    IvVector3 GetPickRay( float sx, float sy, float fov, float width, float height );

    IvMatrix44 mWorldToViewMatrix;
    IvMatrix44 mViewToWorldMatrix;
    IvVector3  mEyePoint;

    IvVector3  mClickPosition;

private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
