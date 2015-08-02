//===============================================================================
// @ IvBoundingSphere.h
// 
// Bounding sphere class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvBoundingSphere__h__
#define __IvBoundingSphere__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvQuat;
class IvLine3;
class IvMatrix33;
class IvRay3;
class IvLineSegment3;
class IvPlane;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvBoundingSphere
{
public:
    // constructor/destructor
    inline IvBoundingSphere() :
        mCenter( 0.0f, 0.0f, 0.0f ), mRadius( 1.0f )
    {}
    inline IvBoundingSphere( const IvVector3& center, float radius ) :
        mCenter( center ), mRadius( radius )
    {}
    inline ~IvBoundingSphere() {}

    // copy operations
    IvBoundingSphere( const IvBoundingSphere& other );
    IvBoundingSphere& operator=( const IvBoundingSphere& other );

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvBoundingSphere& source );

    // accessors
    inline const IvVector3& GetCenter() const { return mCenter; }
    inline float GetRadius() const { return mRadius; }

    // comparison
    bool operator==( const IvBoundingSphere& other ) const;
    bool operator!=( const IvBoundingSphere& other ) const;

    // manipulators
    inline void SetCenter( const IvVector3& center )  { mCenter = center; }
    inline void SetRadius( float radius )  { mRadius = radius; }
    void Set( const IvVector3* points, unsigned int numPoints );
    void AddPoint( const IvVector3& point );

    // transform!
    IvBoundingSphere Transform( float scale, const IvQuat& rotation, 
                         const IvVector3& translation ) const;
    IvBoundingSphere Transform( float scale, const IvMatrix33& rotation, 
                         const IvVector3& translation ) const;

    // intersection
    bool Intersect( const IvBoundingSphere& other ) const;
    bool Intersect( const IvLine3& line ) const;
    bool Intersect( const IvRay3& ray ) const;
    bool Intersect( const IvLineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const IvPlane& plane ) const;

    // merge
    friend void Merge( IvBoundingSphere& result, 
       const IvBoundingSphere& s0, const IvBoundingSphere& s1 );

    // collision parameters
    bool ComputeCollision( const IvBoundingSphere& other, 
                           IvVector3& collisionNormal, 
                           IvVector3& collisionPoint, 
                           float& penetration ) const;

protected:
    IvVector3       mCenter;
    float           mRadius;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
