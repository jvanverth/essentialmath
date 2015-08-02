//===============================================================================
// @ IvVector3.h
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

#ifndef __IvVector3__h__
#define __IvVector3__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvMatrix33;

class IvVector3
{
    friend class IvLine3;
    friend class IvLineSegment3;
    friend class IvMatrix33;
    friend class IvMatrix44;
    friend class IvPlane;
    friend class IvQuat;
    friend class IvRay3;
    
public:
    // constructor/destructor
    inline IvVector3() {}
    inline IvVector3( float _x, float _y, float _z ) :
        x(_x), y(_y), z(_z)
    {
    }
    inline ~IvVector3() {}

    // copy operations
    IvVector3(const IvVector3& other);
    IvVector3& operator=(const IvVector3& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvVector3& source);

    // accessors
    inline float& operator[]( unsigned int i )          { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const { return (&x)[i]; }

    float Length() const;
    float LengthSquared() const;

    friend float Distance( const IvVector3& p0, const IvVector3& p1 );
    friend float DistanceSquared( const IvVector3& p0, const IvVector3& p1 );

    // comparison
    bool operator==( const IvVector3& other ) const;
    bool operator!=( const IvVector3& other ) const;
    bool IsZero() const;
    bool IsUnit() const;

    // manipulators
    inline void Set( float _x, float _y, float _z );
    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit vector

    // operators

    // addition/subtraction
    IvVector3 operator+( const IvVector3& other ) const;
    friend IvVector3& operator+=( IvVector3& vector, const IvVector3& other );
    IvVector3 operator-( const IvVector3& other ) const;
    friend IvVector3& operator-=( IvVector3& vector, const IvVector3& other );

    IvVector3 operator-() const;

    // scalar multiplication
    IvVector3   operator*( float scalar );
    friend IvVector3    operator*( float scalar, const IvVector3& vector );
    IvVector3&          operator*=( float scalar );
    IvVector3   operator/( float scalar );
    IvVector3&          operator/=( float scalar );

    // dot product/cross product
    float               Dot( const IvVector3& vector ) const;
    friend float        Dot( const IvVector3& vector1, const IvVector3& vector2 );
    IvVector3           Cross( const IvVector3& vector ) const;
    friend IvVector3    Cross( const IvVector3& vector1, const IvVector3& vector2 );

    // matrix products
    friend IvVector3 operator*( const IvVector3& vector, const IvMatrix33& mat );
 
    // useful defaults
    static IvVector3    xAxis;
    static IvVector3    yAxis;
    static IvVector3    zAxis;
    static IvVector3    origin;
    
    // member variables
    float x, y, z;
};

float Distance( const IvVector3& p0, const IvVector3& p1 );
float DistanceSquared( const IvVector3& p0, const IvVector3& p1 );

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVector3::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
IvVector3::Set( float _x, float _y, float _z )
{
    x = _x; y = _y; z = _z;
}   // End of IvVector3::Set()

//-------------------------------------------------------------------------------
// @ IvVector3::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
IvVector3::Zero()
{
    x = y = z = 0.0f;
}   // End of IvVector3::Zero()

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
