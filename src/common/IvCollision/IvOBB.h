//===============================================================================
// @ IvOBB.h
// 
// Object-oriented bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvOBB__h__
#define __IvOBB__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"
#include "IvMatrix33.h"

#include <float.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvLine3;
class IvRay3;
class IvLineSegment3;
class IvPlane;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvOBB
{
public:
    // constructor/destructor
    inline IvOBB() :
        mCenter( 0.0f, 0.0f, 0.0f ), mExtents( 1.0f, 1.0f, 1.0f )
        { mRotation.Identity(); }
    inline ~IvOBB() {}

    // copy operations
    IvOBB( const IvOBB& other );
    IvOBB& operator=( const IvOBB& other );

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvOBB& source );

    // accessors
    inline const IvVector3& GetCenter() const   { return mCenter; }
    inline const IvMatrix33& GetRotation() const { return mRotation; }
    inline const IvVector3& GetExtents() const  { return mExtents; }

    // comparison
    bool operator==( const IvOBB& other ) const;
    bool operator!=( const IvOBB& other ) const;

    // manipulators
    void Set( const IvVector3* points, unsigned int numPoints );
    inline void SetCenter( const IvVector3& c ) { mCenter = c; }
    inline void SetRotation( const IvMatrix33& R ) { mRotation = R; }
    inline void SetExtents( const IvVector3& h ) { mExtents = h; }

    // transform!
    IvOBB Transform( float scale, const IvQuat& rotation, 
                     const IvVector3& translation ) const;
    IvOBB Transform( float scale, const IvMatrix33& rotation, 
                     const IvVector3& translation ) const;

    // intersection
    bool Intersect( const IvOBB& other ) const;
    bool Intersect( const IvLine3& line ) const;
    bool Intersect( const IvRay3& ray ) const;
    bool Intersect( const IvLineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const IvPlane& plane ) const;

    // merge
    friend void Merge( IvOBB& result, const IvOBB& b0, const IvOBB& b1 );

protected:
    IvVector3       mCenter;
    IvMatrix33      mRotation;
    IvVector3       mExtents;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
