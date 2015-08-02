//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

#ifndef __PlayerDefs__
#define __PlayerDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix33.h>
#include <IvVector3.h>

class Turret;
class IvVertexBuffer;
class IvIndexBuffer;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

    bool ReadData();

private:
    void CleanData();

    // transformation
    float       mScale;
    IvMatrix33  mRotate;
    IvVector3   mTranslate;

    // geometry
    IvVertexBuffer* mVertices;
    IvIndexBuffer*  mIndices;

    // children
    Turret*     mTurret;
    
};

#endif
