//===============================================================================
// @ IvCapsule.h
// 
// Capsule class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvCapsule__h__
#define __IvCapsule__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvLineSegment3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class IvLine3;
class IvMatrix33;
class IvRay3;
class IvPlane;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvCapsule
{
public:
    // constructor/destructor
    inline IvCapsule() {}
    inline IvCapsule( const IvVector3& endpoint0, const IvVector3& endpoint1, float radius ) :
        mSegment( endpoint0, endpoint1 ), mRadius( radius )
    {
    }
    inline IvCapsule( const IvLineSegment3& segment, float radius ) :
        mSegment( segment ), mRadius( radius )
    {
    }
    inline ~IvCapsule() {}

    // copy operations
    IvCapsule( const IvCapsule& other );
    IvCapsule& operator=( const IvCapsule& other );

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvCapsule& source );

    // accessors
    inline const IvLineSegment3& GetSegment() const { return mSegment; }
    inline float GetRadius() const { return mRadius; }

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const IvCapsule& other ) const;
    bool operator!=( const IvCapsule& other ) const;

    // manipulators
    inline void SetSegment( const IvLineSegment3& segment )  { mSegment = segment; }
    inline void SetRadius( float radius )  { mRadius = radius; }

    void Set( const IvVector3* points, unsigned int numPoints );

    // transform!
    IvCapsule Transform( float scale, const IvQuat& rotation, 
                         const IvVector3& translation ) const;
    IvCapsule Transform( float scale, const IvMatrix33& rotation, 
                         const IvVector3& translation ) const;

    // intersection
    bool Intersect( const IvCapsule& other ) const;
    bool Intersect( const IvLine3& line ) const;
    bool Intersect( const IvRay3& ray ) const;
    bool Intersect( const IvLineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const IvPlane& plane ) const;

    // merge 
    friend void Merge( IvCapsule& result, const IvCapsule& b0, const IvCapsule& b1 );

    // collision parameters
    bool ComputeCollision( const IvCapsule& other, 
                           IvVector3& collisionNormal, 
                           IvVector3& collisionPoint, 
                           float& penetration ) const;

protected:
    IvLineSegment3  mSegment;
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
