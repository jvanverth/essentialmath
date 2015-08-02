//===============================================================================
// @ Barrel.cpp
// ------------------------------------------------------------------------------
// Barrel class
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvFileReader.h>
#include <IvRendererHelp.h>
#include <IvMath.h>
#include <IvMatrix44.h>
#include <IvResourceManager.h>
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>

#include "Barrel.h"
#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Barrel::Barrel()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Barrel::Barrel()
{
    mRotate.Identity();
    mScale = 1.0f;
    mTranslate.Set( 4.5f, 0.0f, 1.0f );

}   // End of Barrel::Barrel()


//-------------------------------------------------------------------------------
// @ Barrel::~Barrel()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Barrel::~Barrel()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
    mVertices = 0;
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
    mIndices = 0;

}   // End of Barrel::~Barrel()


//-------------------------------------------------------------------------------
// @ Barrel::ReadData()
//-------------------------------------------------------------------------------
// Read in geometry data
//-------------------------------------------------------------------------------
bool
Barrel::ReadData()
{
    // read in geometric data
    IvFileReader in("Barrel.txt");
    if (!in)
        return false;

    // get relative transform
    in >> mScale;

    for (unsigned int j = 0; j < 3; j++)
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            in >> mRotate(i,j);
        }
    }

    float x, y, z;
    in >> x >> y >> z;
    mTranslate.Set(x,y,z);
    
    unsigned int numVerts;
    
    // get number of vertices
    in >> numVerts;
    if ( !in.good() )
        return false;

    mVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCNPFormat, numVerts,
                                                                                NULL, kDefaultUsage);
    IvCNPVertex* dataPtr = (IvCNPVertex*) mVertices->BeginLoadData();

    // read positions
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float x, y, z;
        in >> x >> y >> z;
        dataPtr[i].position.Set(x,y,z);
        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }

    // read normals
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float x, y, z;
        in >> x >> y >> z;
        dataPtr[i].normal.Set(x,y,z);
        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }

    // read colors
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float r, g, b;
        in >> r >> g >> b;
        dataPtr[i].color.mRed = UChar8(r*255);
        dataPtr[i].color.mGreen = UChar8(g*255);
        dataPtr[i].color.mBlue = UChar8(b*255);
        dataPtr[i].color.mAlpha = 255;

        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }

    if (!mVertices->EndLoadData())
    {
        CleanData();
        return false;
    }

    // get number of indices
    UInt32 numTankBodyIndices;
    in >> numTankBodyIndices;
    if ( !in.good() )
        return false;

    // read indices
    mIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numTankBodyIndices,
                                                                              NULL, kDefaultUsage);
    UInt32* indexPtr = static_cast<UInt32*>(mIndices->BeginLoadData());
    for ( UInt32 i = 0; i < numTankBodyIndices; ++i )
    {
        in >> indexPtr[i];
        if (!in.good() )
        {
            CleanData();
            return false;
        }
    }
    if (!mIndices->EndLoadData())
    {
        CleanData();
        return false;
    }
    
    return true;

}   // End of Barrel::ReadData()


//-------------------------------------------------------------------------------
// @ Barrel::CleanData()
//-------------------------------------------------------------------------------
// Clean geometry data
//-------------------------------------------------------------------------------
void 
Barrel::CleanData()
{
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
    mVertices = 0;
    IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
    mIndices = 0;

}   // End of Barrel::CleanData()


//-------------------------------------------------------------------------------
// @ Barrel::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Barrel::Update( float dt )
{
    float r = 0.0f;
    IvMatrix33 rotate;
    IvVector3 xlate;
    IvMatrix44 transform;
    
    // set up rotate
    if (IvGame::mGame->mEventHandler->IsKeyDown('w'))
    {
        r -= kPI*0.125f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('s'))
    {
        r += kPI*0.125f*dt;
    }
    rotate.RotationY(r);
    mRotate = mRotate*rotate;
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyDown(' '))
    {
        mRotate.Identity();
    }

}   // End of Barrel::Update()


//-------------------------------------------------------------------------------
// @ Barrel::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Barrel::Render( float parentScale, 
             const IvMatrix33& parentRotate, 
             const IvVector3& parentTranslation )                               
{   
    float scale = parentScale*mScale;
    IvMatrix33 rotate = parentRotate*mRotate; 
    IvVector3 translate = parentTranslation + parentScale*parentRotate*mTranslate;

    // build 4x4 matrix
    IvMatrix44 transform;

    transform.Identity();
    (void) transform.Rotation( rotate );
    transform(0,0) *= scale;
    transform(1,0) *= scale;
    transform(2,0) *= scale;
    transform(0,1) *= scale;
    transform(1,1) *= scale;
    transform(2,1) *= scale;
    transform(0,2) *= scale;
    transform(1,2) *= scale;
    transform(2,2) *= scale;
    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;
    
    IvSetWorldMatrix( transform );
    IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices, mIndices);
}


