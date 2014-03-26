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


class Game : public IvGame
{
public:
    Game();
    ~Game();
    bool PostRendererInitialize();

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();

    void DrawScene();

    void Frustum( float left, float right, 
                  float bottom, float top, 
                  float nearZ, float farZ );

    void SetCamera( const IvVector3& viewDir, 
                    const IvVector3& viewSide, 
                    const IvVector3& viewPos );    

    IvVector3   mViewPoint;
    IvVector3   mViewDirection;
    float       mFocalLength;
    float       mEyeSeparation;
    
private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
