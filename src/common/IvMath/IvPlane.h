//===============================================================================
// @ IvPlane.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvPlane__h__
#define __IvPlane__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"
#include "IvMath.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class IvQuat;
class IvLine3;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvPlane
{
public:
    // constructor/destructor
    IvPlane();
    IvPlane( float a, float b, float c, float d );
    IvPlane( const IvVector3& p0, const IvVector3& p1, const IvVector3& p2 );
    inline ~IvPlane() {}
    
    // copy operations
    IvPlane(const IvPlane& other);
    IvPlane& operator=(const IvPlane& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvPlane& source);

    // accessors
    inline const IvVector3& GetNormal() const { return mNormal; }
    inline float GetOffset() const { return mOffset; }
    void Get( IvVector3& normal, float& direction ) const;

    // comparison
    bool operator==( const IvPlane& ray ) const;
    bool operator!=( const IvPlane& ray ) const;

    // manipulators
    void Set( float a, float b, float c, float d );
    inline void Set( const IvVector3& n, float d ) { Set( n.x, n.y, n.z, d ); }
    void Set( const IvVector3& p0, const IvVector3& p1, const IvVector3& p2 );

    // transform!
    IvPlane Transform( float scale, const IvQuat& rotation, 
                       const IvVector3& translation ) const;

    // distance
    inline friend float Distance( const IvPlane& plane, const IvVector3& point )
    {
        return ::IvAbs( plane.Test( point ) );
    }

    // closest point
    IvVector3 ClosestPoint( const IvVector3& point ) const;

    // result of plane test
    inline float Test( const IvVector3& point ) const
    {
        return mNormal.Dot(point) + mOffset;
    }
        
protected:
    IvVector3 mNormal;
    float     mOffset;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
