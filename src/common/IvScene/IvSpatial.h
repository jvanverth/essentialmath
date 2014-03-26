//===============================================================================
// @ IvSpatial.h
// 
// Abstract scene graph object that supports hierarchical transforms
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

#ifndef __IvSpatial__h__
#define __IvSpatial__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include <IvMatrix33.h>
#include <IvVector3.h>
#include <IvBoundingSphere.h>
#include <IvWriter.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvSpatial
{
public:
    // constructor/destructor
    IvSpatial();
    virtual ~IvSpatial();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvSpatial& source );

    virtual void UpdateWorldTransform();

    virtual void Render() = 0;

    // accessors
    void SetLocalScale(float s) { mLocalScale = s; }
    float GetLocalScale() const { return mLocalScale; }
    
    void SetLocalRotate(const IvMatrix33& r) { mLocalRotate = r; }
    const IvMatrix33& GetLocalRotate() const { return mLocalRotate; }
    
    void SetLocalTranslate(const IvVector3& t) { mLocalTranslate = t; }
    const IvVector3& GetLocalTranslate() const { return mLocalTranslate; }
    
    float GetWorldScale() const { return mWorldScale; }
    
    const IvMatrix33& GetWorldRotate() const { return mWorldRotate; }
    
    const IvVector3& GetWorldTranslate() const { return mWorldTranslate; }

    IvSpatial* GetParent() { return mParent; }
    const IvSpatial* GetParent() const { return mParent; }
    void SetParent(IvSpatial* parent) { mParent = parent; }
    
    const IvBoundingSphere& GetWorldBoundingSphere() const 
    { return mWorldSphere; }

    static bool gDisplayHierarchyBounds;
    static bool gDisplayLeafBounds;

protected:
    IvSpatial* mParent;

    float       mLocalScale;
    IvMatrix33  mLocalRotate;
    IvVector3   mLocalTranslate;

    float       mWorldScale;
    IvMatrix33  mWorldRotate;
    IvVector3   mWorldTranslate;

    IvBoundingSphere mWorldSphere;
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
