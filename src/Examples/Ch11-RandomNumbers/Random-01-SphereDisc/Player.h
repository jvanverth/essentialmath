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

class IvVertexBuffer;

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    IvVector3 GenSpherePointIncorrect();
    IvVector3 GenSpherePointCorrect();
    IvVector3 GenDiscPointIncorrect();
    IvVector3 GenDiscPointCorrect();

    float       mScale;
    IvQuat      mRotate;
    IvVector3   mTranslate;  

    unsigned int mMode;

    IvVertexBuffer* mVertices;
};

#endif
