//===============================================================================
// @ IvHierarchy.h
// 
// Container object that supports hierarchical transforms
//
// The hierarchy is managed via a 'structure of arrays' approach, rather than
// the more traditionally objected-oriented tree of separately allocated nodes.
// This gives better cache coherency during updates and rendering.
// ------------------------------------------------------------------------------
// Copyright (C) 2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvHierarchy__h__
#define __IvHierarchy__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvBoundingSphere.h>
#include <IvQuat.h>
#include <IvReader.h>
#include <IvVector3.h>
#include <IvWriter.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvIndexedGeometry;
class IvBoundingSphere;
class IvCapsule;

class IvHierarchy
{
public:
    // constructor/destructor
    IvHierarchy();
    ~IvHierarchy();

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvHierarchy& source);

    // builders
    bool AllocNodes(int count);
    bool AddNode(int index, unsigned char parent, 
                 IvReader& inStream,
                 const IvVector3& xlate, const IvQuat& rotate, float scale);

    // regular updates
    void UpdateWorldTransforms();
    void Render();

    // accessors
    inline void SetLocalScale(float s, int i) { mLocalTransforms[i].mScale = s; }
    inline float GetLocalScale(int i) const { return mLocalTransforms[i].mScale; }

    inline void SetLocalRotate(const IvQuat& r, int i) { mLocalTransforms[i].mRotate = r; }
    inline const IvQuat& GetLocalRotate(int i) const { return mLocalTransforms[i].mRotate; }

    inline void SetLocalTranslate(const IvVector3& t, int i) { mLocalTransforms[i].mTranslate = t; }
    inline const IvVector3& GetLocalTranslate(int i) const { return mLocalTransforms[i].mTranslate; }

    inline float GetWorldScale(int i) const { return mWorldTransforms[i].mScale; }
    inline const IvQuat& GetWorldRotate(int i) const { return mWorldTransforms[i].mRotate; }
    inline const IvVector3& GetWorldTranslate(int i) const { return mWorldTransforms[i].mTranslate; }

    inline const IvBoundingSphere& GetWorldBoundingSphere(int i) const
    {
        return mWorldSpheres[i];
    }

    static bool gDisplayHierarchyBounds;
    static bool gDisplayLeafBounds;

protected:
    struct Transform
    {
        IvVector3 mTranslate;
        float     mScale;
        IvQuat    mRotate;
    };

    int            mNumNodes;
    unsigned char* mParents;
    Transform*     mLocalTransforms;
    Transform*     mWorldTransforms;

    IvBoundingSphere* mWorldSpheres;

    IvCapsule*     mLocalCapsules;
    IvCapsule*     mWorldCapsules;

    IvIndexedGeometry* mGeometries;
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
