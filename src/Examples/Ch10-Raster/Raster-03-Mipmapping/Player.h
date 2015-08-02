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

class IvIndexBuffer;
class IvShaderProgram;
class IvTexture;
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
    enum
    {
        FILTER_BILERP,
        FILTER_MIPMAP,
        FILTER_TRILERP,
        NUM_FILTERS
    };

    void DrawQuad();
    void CreateQuad();

    float       mScale;
    IvMatrix33  mRotate;

    unsigned int mFilter;
    IvTexture* mMipmapLevelBandsTex;
    IvTexture* mCheckerBoardTex;

    IvIndexBuffer* mQuadIndices;
    IvVertexBuffer* mQuadVerts;

    IvShaderProgram* mShader;
    IvUniform* mTextureUniform;
};

#endif
