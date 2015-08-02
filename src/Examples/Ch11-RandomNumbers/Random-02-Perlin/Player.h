//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//
//
//===============================================================================

#ifndef __PlayerDefs__
#define __PlayerDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvQuat.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvShaderProgram;

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    float       mScale;
    IvQuat      mRotate;
    IvVector3   mTranslate;

    bool        mUseSkyShader;

    IvShaderProgram* mSkyShader;
    IvShaderProgram* mMarbleShader;
    
};

#endif
