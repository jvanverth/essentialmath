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

    IvIndexBuffer* mPlaneIndices;
    IvVertexBuffer* mPlaneVerts;

    IvVector3 mLightPos;

    IvShaderProgram* mFraglitShader;
    IvShaderProgram* mVertlitShader;
    IvShaderProgram* mShader;
};

#endif
