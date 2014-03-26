//===============================================================================
// @ IvAABB.h
// 
// Axis-aligned bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvAABB__h__
#define __IvAABB__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvWriter.h>
#include <IvVector3.h>

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

class IvAABB
{
public:
    // constructor/destructor
    inline IvAABB() :
        mMinima( FLT_MAX, FLT_MAX, FLT_MAX ), mMaxima( FLT_MIN, FLT_MIN, FLT_MIN )
    {}
    inline IvAABB( const IvVector3& min, const IvVector3& max ) :
        mMinima( min ), mMaxima( max )
    {}
    inline ~IvAABB() {}

    // copy operations
    IvAABB( const IvAABB& other );
    IvAABB& operator=( const IvAABB& other );

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvAABB& source );

    // accessors
    inline const IvVector3& GetMinima() const { return mMinima; }
    inline const IvVector3& GetMaxima() const { return mMaxima; }

    // comparison
    bool operator==( const IvAABB& other ) const;
    bool operator!=( const IvAABB& other ) const;

    // manipulators
    void Set( const IvVector3* points, unsigned int numPoints );
    inline void Set( const IvVector3& min, const IvVector3& max );
    void AddPoint( const IvVector3& point );

    // intersection
    bool Intersect( const IvAABB& other ) const;
    bool Intersect( const IvLine3& line ) const;
    bool Intersect( const IvRay3& ray ) const;
    bool Intersect( const IvLineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const IvPlane& plane ) const;

    // merge
    friend void Merge( IvAABB& result, const IvAABB& b0, const IvAABB& b1 );

protected:
    IvVector3       mMinima, mMaxima;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

inline void 
IvAABB::Set( const IvVector3& min, const IvVector3& max )
{
    mMinima = min;
    mMaxima = max;
}

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
