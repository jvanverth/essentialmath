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
#include "SimObject.h"

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class Player;
class Obstacle;

const unsigned int kMaxObjects = 128;

class Game : public IvGame
{
public:
    Game();
    ~Game();

    virtual bool PostRendererInitialize();
    
    SimObject       mObjects[kMaxObjects];
    unsigned int    mNumObjects;

    Player*         mPlayer;
    
    bool            mUseCollision;

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();
    
private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
