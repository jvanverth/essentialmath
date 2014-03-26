//===============================================================================
// @ IvVector2.h
// 
// 2D vector class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvVector2__h__
#define __IvVector2__h__

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

class IvVector2
{
public:
    // constructor/destructor
    inline IvVector2() {}
    inline IvVector2( float _x, float _y ) :
        x(_x), y(_y)
    {
    }
    inline ~IvVector2() {}
    
    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvVector2& source);
    
    // accessors
    inline float& operator[]( unsigned int i )          { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const { return (&x)[i]; }

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const IvVector2& other ) const;
    bool operator!=( const IvVector2& other ) const;
    bool IsZero() const;

    // manipulators
    inline void Set( float _x, float _y );
    inline void SetX( float _x ) { x = _x; }
    inline void SetY( float _y ) { y = _y; }
    inline float GetX() const { return x; }
    inline float GetY() const { return y; }
    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit vector

    // operators
    IvVector2 operator-() const;

    // addition/subtraction
    IvVector2 operator+( const IvVector2& other ) const;
    friend IvVector2& operator+=( IvVector2& self, const IvVector2& other );
    IvVector2 operator-( const IvVector2& other ) const;
    friend IvVector2& operator-=( IvVector2& self, const IvVector2& other );


    // scalar multiplication
    IvVector2   operator*( float scalar );
    friend IvVector2    operator*( float scalar, const IvVector2& vector );
    IvVector2&          operator*=( float scalar );
    IvVector2   operator/( float scalar );
    IvVector2&          operator/=( float scalar );

    // dot product
    float               Dot( const IvVector2& vector ) const;
    friend float        Dot( const IvVector2& vector1, const IvVector2& vector2 );

    // perpendicular and cross product equivalent
    IvVector2 Perpendicular() const { return IvVector2(-y, x); } 
    float               PerpDot( const IvVector2& vector ) const; 
    friend float        PerpDot( const IvVector2& vector1, const IvVector2& vector2 );

    // useful defaults
    static IvVector2    xAxis;
    static IvVector2    yAxis;
    static IvVector2    origin;
    static IvVector2    xy;
        
protected:
    // member variables
    float x, y;
        
private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVector2::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
IvVector2::Set( float _x, float _y )
{
    x = _x; y = _y;
}   // End of IvVector2::Set()

//-------------------------------------------------------------------------------
// @ IvVector2::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
IvVector2::Zero()
{
    x = y = 0.0f;
}   // End of IvVector2::Zero()

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
