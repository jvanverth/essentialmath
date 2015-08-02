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
class IvShaderProgram;
class IvUniform;
class IvVertexBuffer;

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    void CreateStrips();
    void DrawStrips();

    float       mScale;
    IvMatrix33  mRotate;
    IvVector3   mTranslate;

    IvIndexBuffer* mSmoothStripIndices;
    IvVertexBuffer* mSmoothStripVerts;

    IvIndexBuffer* mSharpStripIndices;
    IvVertexBuffer* mSharpStripVerts;

    IvShaderProgram* mShader;
};

#endif
