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
#include <IvTypes.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class Player;

class Game : public IvGame
{
public:
    Game();
    ~Game();
    bool PostRendererInitialize();

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();

    void Orthographic( float left, float right, 
                  float bottom, float top, 
                  float nearZ, float farZ );
    void Frustum( float left, float right, 
                  float bottom, float top, 
                  float nearZ, float farZ );
    
    float mXCenter;
    float mYCenter;

    bool  mUseOrthographic;
    
private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
