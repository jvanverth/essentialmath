//===============================================================================
// @ IvHierarchy.cpp
// 
// Container object that supports hierarchical transforms
// ------------------------------------------------------------------------------
// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvHierarchy.h"

#include <IvAssert.h>
#include <IvCapsule.h>
#include "IvIndexedGeometry.h"
#include <IvMatrix44.h>
#include <IvRenderer.h>
#include <IvRendererHelp.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

bool IvHierarchy::gDisplayLeafBounds = false;
bool IvHierarchy::gDisplayHierarchyBounds = false;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvHierarchy::IvHierarchy()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
IvHierarchy::IvHierarchy() 
    : mNumNodes(0)
    , mParents(NULL)
    , mLocalTransforms(NULL)
    , mWorldTransforms(NULL)
    , mWorldSpheres(NULL)
    , mLocalCapsules(NULL)
    , mWorldCapsules(NULL)
    , mGeometries(NULL)
{
}  // End of IvHierarchy::IvHierarchy


//-------------------------------------------------------------------------------
// @ IvHierarchy::~IvHierarchy()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
IvHierarchy::~IvHierarchy()
{
    delete[] mParents;
    delete[] mLocalTransforms;
    delete[] mWorldTransforms;
    delete[] mWorldSpheres;
    delete[] mLocalCapsules;
    delete[] mWorldCapsules;
    for (int i = 0; i < mNumNodes; ++i)
    {
        mGeometries[i].FreeResources();
    }
    delete[] mGeometries;
    mNumNodes = 0;

}  // End of IvHierarchy::~IvHierarchy


//-------------------------------------------------------------------------------
// @ IvHierarchy::AllocNodes()
//-------------------------------------------------------------------------------
// Alloc the arrays for the nodes
//-------------------------------------------------------------------------------
bool IvHierarchy::AllocNodes(int count)
{
    mNumNodes = count;
    mParents = new unsigned char[count];
    mLocalTransforms = new Transform[count];
    mWorldTransforms = new Transform[count];
    mWorldSpheres = new IvBoundingSphere[count];
    mLocalCapsules = new IvCapsule[count];
    mWorldCapsules = new IvCapsule[count];
    mGeometries = new IvIndexedGeometry[count];

    return true;
}

//-------------------------------------------------------------------------------
// @ IvHierarchy::AddNode()
//-------------------------------------------------------------------------------
// Add node data to the tree
//-------------------------------------------------------------------------------
bool IvHierarchy::AddNode(int index, unsigned char parent,
                          IvReader& inStream,
                          const IvVector3& xlate, const IvQuat& rotate, float scale)
{
    if (mNumNodes == 0 || index >= mNumNodes)
    {
        return false;
    }
    // child nodes must be later in the array than their parents
    if (!((index == 0 && parent == 0) || (parent < index)))
    {
        return false;
    }
    mParents[index] = parent;
    mGeometries[index].LoadFromStream(inStream, mLocalCapsules[index]);
    mLocalTransforms[index].mRotate = rotate;
    mLocalTransforms[index].mTranslate = xlate;
    mLocalTransforms[index].mScale = scale;

    return true;
}

//-------------------------------------------------------------------------------
// @ IvHierarchy::UpdateWorldTransforms()
//-------------------------------------------------------------------------------
// Propagate transforms down and back up the hierarchy
//-------------------------------------------------------------------------------
void IvHierarchy::UpdateWorldTransforms()
{
    ASSERT(mNumNodes > 0);
    ASSERT(mParents[0] == 0);  // root node should be 0
    mWorldTransforms[0] = mLocalTransforms[0];
    mWorldTransforms[0].mRotate.Normalize();
    mWorldCapsules[0] = mLocalCapsules[0].Transform(mWorldTransforms[0].mScale,
                                                    mWorldTransforms[0].mRotate,
                                                    mWorldTransforms[0].mTranslate);
    mWorldSpheres[0].SetCenter(mWorldCapsules[0].GetSegment().GetCenter());
    mWorldSpheres[0].SetRadius(mWorldCapsules[0].GetSegment().Length() * 0.5f
                             + mWorldCapsules[0].GetRadius());

    // propagate down to children
    for (int i = 1; i < mNumNodes; ++i)
    {
        int parent = mParents[i];
        ASSERT(parent < i);
        // combine parent transform with our own
        float parentScale = mWorldTransforms[parent].mScale;
        mWorldTransforms[i].mTranslate = 
            mWorldTransforms[parent].mRotate.Rotate(mLocalTransforms[i].mTranslate);
        mWorldTransforms[i].mTranslate *= parentScale;
        mWorldTransforms[i].mTranslate += mWorldTransforms[parent].mTranslate;
        mWorldTransforms[i].mScale = mLocalTransforms[i].mScale*parentScale;
        mWorldTransforms[i].mRotate = mWorldTransforms[parent].mRotate*mLocalTransforms[i].mRotate;
        mWorldTransforms[i].mRotate.Normalize();

        // transform local capsule into world space
        mWorldCapsules[i] = mLocalCapsules[i].Transform(mWorldTransforms[i].mScale,
                                                        mWorldTransforms[i].mRotate,
                                                        mWorldTransforms[i].mTranslate);

        // generate world space bounding sphere
        mWorldSpheres[i].SetCenter(mWorldCapsules[i].GetSegment().GetCenter());
        mWorldSpheres[i].SetRadius(mWorldCapsules[i].GetSegment().Length() * 0.5f
                                 + mWorldCapsules[i].GetRadius());
    }

    // merge bounding spheres up to parents
    for (int i = mNumNodes - 1; i > 0; --i)
    {
        int parent = mParents[i];
        Merge(mWorldSpheres[parent], mWorldSpheres[parent], mWorldSpheres[i]);
    }

}  // End of IvHierarchy::UpdateWorldTransforms


//-------------------------------------------------------------------------------
// @ IvHierarchy::Render()
//-------------------------------------------------------------------------------
// Draws the entire hierarchy
//-------------------------------------------------------------------------------
void IvHierarchy::Render()
{
    for (int i = 0; i < mNumNodes; ++i)
    {
        // build 4x4 matrix
        IvMatrix44 transform(mWorldTransforms[i].mRotate);
        transform(0, 0) *= mWorldTransforms[i].mScale;
        transform(1, 0) *= mWorldTransforms[i].mScale;
        transform(2, 0) *= mWorldTransforms[i].mScale;
        transform(0, 1) *= mWorldTransforms[i].mScale;
        transform(1, 1) *= mWorldTransforms[i].mScale;
        transform(2, 1) *= mWorldTransforms[i].mScale;
        transform(0, 2) *= mWorldTransforms[i].mScale;
        transform(1, 2) *= mWorldTransforms[i].mScale;
        transform(2, 2) *= mWorldTransforms[i].mScale;
        transform(0, 3) = mWorldTransforms[i].mTranslate.x;
        transform(1, 3) = mWorldTransforms[i].mTranslate.y;
        transform(2, 3) = mWorldTransforms[i].mTranslate.z;

        // set current local-to-world matrix
        IvSetWorldMatrix(transform);

        mGeometries[i].Render();

        if (gDisplayLeafBounds)
        {
            IvRenderer::mRenderer->SetFillMode(kWireframeFill);

            IvSetWorldIdentity();
            IvDrawCapsule(mWorldCapsules[i].GetSegment(), mWorldCapsules[i].GetRadius(), kOrange);
            IvRenderer::mRenderer->SetFillMode(kSolidFill);
        }

        if (gDisplayHierarchyBounds)
        {
            IvRenderer::mRenderer->SetFillMode(kWireframeFill);
            IvMatrix44 ident;
            ident.Identity();
            ident(0, 3) = mWorldSpheres[i].GetCenter().x;
            ident(1, 3) = mWorldSpheres[i].GetCenter().y;
            ident(2, 3) = mWorldSpheres[i].GetCenter().z;

            IvSetWorldMatrix(ident);
            IvDrawSphere(mWorldSpheres[i].GetRadius(), kYellow);

            IvRenderer::mRenderer->SetFillMode(kSolidFill);
        }
    }

}  // End of IvHierarchy::Render
