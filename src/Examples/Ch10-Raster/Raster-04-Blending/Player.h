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
#include <IvRenderer.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvIndexBuffer;
class IvTexture;
class IvVertexBuffer;
class IvShaderProgram;
class IvUniform;

class Player
{
public:
    Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    void CreateCylinder();
    void DrawCylinder();

    float       mScale;
    IvMatrix33  mRotate;
    IvVector3   mTranslate;

    float mRadius;

    enum { NUM_BLEND_TEX = 3 };

    IvIndexBuffer* mCylinderIndices;
    IvVertexBuffer* mCylinderVerts;

    IvTexture* mBaseTexture;
    IvTexture* mBlendTextures[NUM_BLEND_TEX];
    unsigned int mCurrentBlendTexIndex;

    enum BlendMode {
        kNoneBlendMode,
        kOpacityBlendMode,
        kAddBlendMode,
        kModulateBlendMode,

        kLastBlendMode = kModulateBlendMode
    }; 
    const int kBlendModeCount = kLastBlendMode + 1;
    BlendMode mBlendMode;

    void SetBlendFuncs(BlendMode mode);

    IvShaderProgram* mShader;
    IvUniform* mTextureUniform;
};

#endif
