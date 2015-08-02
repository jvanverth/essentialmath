//===============================================================================
// @ IvLine3.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvLine3__h__
#define __IvLine3__h__

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

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvLine3
{
public:
    // constructor/destructor
    IvLine3();
    IvLine3( const IvVector3& origin, const IvVector3& direction );
    inline ~IvLine3() {}
    
    // copy operations
    IvLine3(const IvLine3& other);
    IvLine3& operator=(const IvLine3& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvLine3& source);

    // accessors
    inline IvVector3 GetOrigin() const { return mOrigin; }
    inline IvVector3 GetDirection() const { return mDirection; }
    void Get( IvVector3& origin, IvVector3& direction ) const;

    // comparison
    bool operator==( const IvLine3& line ) const;
    bool operator!=( const IvLine3& line ) const;

    // manipulators
    void Set( const IvVector3& origin, const IvVector3& direction );
    inline void Clean() { mOrigin.Clean(); mDirection.Clean(); }

    // transform!
    IvLine3 Transform( float scale, const IvQuat& rotation, 
                              const IvVector3& translation ) const;

    // distance
    friend float DistanceSquared( const IvLine3& line0, const IvLine3& line1, 
                                  float& s_c, float& t_c );
    inline friend float Distance( const IvLine3& line0, const IvLine3& line1, 
                           float& s_c, float& t_c )
    {
        return IvSqrt( DistanceSquared( line0, line1, s_c, t_c ) );
    }
    friend float DistanceSquared( const IvLine3& line, const IvVector3& point, 
                                  float &t_c );
    inline friend  float Distance( const IvLine3& line, const IvVector3& point, 
                           float &t_c )
    {
        return IvSqrt( DistanceSquared( line, point, t_c ) );
    }

    // closest points
    friend void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                               const IvLine3& line0, 
                               const IvLine3& line1 );
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
