//===============================================================================
// @ Player.h
// ------------------------------------------------------------------------------
// Main player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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
    void DrawCylinder();
    void CreateCylinderVertexArrays();
    void DrawCylinderVertexArrays();

    float       mScale;
    IvMatrix33  mRotate;
    IvVector3   mTranslate;

    float mRadius;

    enum { NUM_UVS = 2 };
    enum { NUM_TEX = 2 };

    IvIndexBuffer* mCylinderIndices;
    IvVertexBuffer* mCylinderVerts[NUM_UVS];
    unsigned int mCurrentUVIndex;

    IvTexture* mTextures[NUM_TEX];
    unsigned int mCurrentTexIndex;

    bool mHorizontalWrap;
    bool mVerticalWrap;

    IvShaderProgram* mShader;
};

#endif
