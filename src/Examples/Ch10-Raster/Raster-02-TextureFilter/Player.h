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
    void DrawQuad();
    void CreateQuad();

    float       mScale;
    IvMatrix33  mRotate;

    IvIndexBuffer* mQuadIndices;
    IvVertexBuffer* mQuadVerts;

    IvTexture* mTexture;
    IvTexture* mCheckerTexture;

    IvShaderProgram* mShader;

    bool mBilerp;
    bool mCheckerboard;
};

#endif
