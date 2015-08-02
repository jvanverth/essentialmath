//===============================================================================
// @ IvRay3.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
//===============================================================================

#ifndef __IvRay3__h__
#define __IvRay3__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class IvQuat;
class IvLine3;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvRay3
{
public:
    // constructor/destructor
    IvRay3();
    IvRay3( const IvVector3& origin, const IvVector3& direction );
    inline ~IvRay3() {}
    
    // copy operations
    IvRay3(const IvRay3& other);
    IvRay3& operator=(const IvRay3& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvRay3& source);

    // accessors
    inline IvVector3 GetOrigin() const { return mOrigin; }
    inline IvVector3 GetDirection() const { return mDirection; }
    void Get( IvVector3& origin, IvVector3& direction ) const;

    // comparison
    bool operator==( const IvRay3& ray ) const;
    bool operator!=( const IvRay3& ray ) const;

    // manipulators
    void Set( const IvVector3& origin, const IvVector3& direction );
    inline void Clean() { mOrigin.Clean(); mDirection.Clean(); }

    // transform!
    IvRay3 Transform( float scale, const IvQuat& rotation, 
                              const IvVector3& translation ) const;

    // distance
    friend float DistanceSquared( const IvRay3& ray0, const IvRay3& ray1,
                                  float& s_c, float& t_c );
    friend float DistanceSquared( const IvRay3& ray, const IvLine3& line, 
                                  float& s_c, float& t_c );
    inline friend float DistanceSquared( const IvLine3& line, const IvRay3& ray, 
                                  float& s_c, float& t_c )
    {
        return DistanceSquared( ray, line, t_c, s_c );
    }
    friend float DistanceSquared( const IvRay3& ray, const IvVector3& point, 
                                  float& t_c );

    // closest points
    friend void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                               const IvRay3& ray0, 
                               const IvRay3& ray1 );
    friend void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                               const IvRay3& ray, 
                               const IvLine3& line );
    IvVector3 ClosestPoint( const IvVector3& point ) const;
        
protected:
    IvVector3 mOrigin;
    IvVector3 mDirection;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
