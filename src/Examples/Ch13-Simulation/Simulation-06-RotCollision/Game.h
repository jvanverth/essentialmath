//===============================================================================
// @ Game.h
// ------------------------------------------------------------------------------
// Game core
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

#ifndef __GameDefs__
#define __GameDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvGame.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class Player;
class Obstacle;

class Game : public IvGame
{
public:
    Game();
    ~Game();

    virtual bool PostRendererInitialize();

    Player*         mPlayer;
    Obstacle*       mObstacle;

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();
    
private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
