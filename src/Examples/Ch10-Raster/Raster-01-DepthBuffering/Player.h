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
#include <IvVector3.h>

class IvIndexBuffer;
class IvShaderProgram;
class IvUniform;
class IvVertexBuffer;

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

private:
    void DrawQuad();
    void CreateQuad();

    float       mScale;
    IvVector3   mTranslate;
    IvMatrix33  mRotate;

    IvVertexBuffer* mQuadVerts;
    IvIndexBuffer* mQuadIndices;
};

#endif
