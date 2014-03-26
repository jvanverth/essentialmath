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
#include <IvVector4.h>

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
    void CreateSphere();
    void DrawSphere();

    float mRadius;

    IvIndexBuffer* mSphereIndices;
    IvVertexBuffer* mSphereVerts;

    IvShaderProgram* mShader;

    enum { NUM_LIGHTS = 2 };

    IvVector4 mLightComponents;

    unsigned int mCurrentLight;
    IvVector4 mLightPos[NUM_LIGHTS];

    IvUniform* mLightPosUniform;
    IvUniform* mViewPosUniform;
};

#endif
