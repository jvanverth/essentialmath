//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __PlayerDefs__
#define __PlayerDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix33.h>
#include <IvVector2.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvIndexBuffer;
class IvTexture;
class IvVertexBuffer;
class IvShaderProgram;

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    void CreatePlane();
    void DrawPlane();

    float       mScale;
    IvMatrix33  mRotate;
    IvVector3   mTranslate;

    enum { NUM_TEX = 2 };

    IvIndexBuffer* mPlaneIndices;
    IvVertexBuffer* mPlaneVerts;

    IvTexture* mTextures[NUM_TEX];
    unsigned int mCurrentTexIndex;

    float mColorScale;

    IvShaderProgram* mShaderClamp;
    IvShaderProgram* mShaderScale;
};

#endif
