//===============================================================================
// @ Game.h
// ------------------------------------------------------------------------------
// Game core
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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
#include <IvTypes.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;

class Game : public IvGame
{
public:
    Game();
    ~Game();
    bool PostRendererInitialize();

    IvVector3   mObjectPosition;

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();

    void LookAt( const IvVector3& eye, const IvVector3& lookAt, const IvVector3& up );  
    
private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
