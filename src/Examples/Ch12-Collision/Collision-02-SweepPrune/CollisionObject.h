//===============================================================================
// @ CollisionObject.h
// ------------------------------------------------------------------------------
// Main CollisionObject
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#ifndef __CollisionObjectDefs__
#define __CollisionObjectDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvVector3.h>
#include <IvBoundingSphere.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class CollisionObject
{
public:
    CollisionObject();
    ~CollisionObject();

    void Update( float dt );
    void Render();

    bool HasCollision( const CollisionObject* other ) const;
    inline void SetColliding( bool colliding ) { mColliding = colliding; }

    inline float GetMinXExtent() { return mPosition.x - mSphere.GetRadius(); }
    inline float GetMaxXExtent() { return mPosition.x + mSphere.GetRadius(); }

private:
    IvVector3           mPosition;
    IvVector3           mVelocity;

    IvBoundingSphere    mSphere;

    bool                mColliding;
};

#endif
