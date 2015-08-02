//===============================================================================
// @ IvVector3.cpp
// 
// 3D vector class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVector3.h"
#include "IvMath.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

IvVector3 IvVector3::xAxis( 1.0f, 0.0f, 0.0f );
IvVector3 IvVector3::yAxis( 0.0f, 1.0f, 0.0f );
IvVector3 IvVector3::zAxis( 0.0f, 0.0f, 1.0f );
IvVector3 IvVector3::origin( 0.0f, 0.0f, 0.0f );

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVector3::IvVector3()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvVector3::IvVector3(const IvVector3& other) : 
    x( other.x ),
    y( other.y ),
    z( other.z )
{

}   // End of IvVector3::IvVector3()


//-------------------------------------------------------------------------------
// @ IvVector3::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvVector3&
IvVector3::operator=(const IvVector3& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    x = other.x;
    y = other.y;
    z = other.z;
    
    return *this;

}   // End of IvVector3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvVector3& source)
{
    out << '<' << source.x << ',' << source.y << ',' << source.z << '>';

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvVector3::Length()
//-------------------------------------------------------------------------------
// Vector length
//-------------------------------------------------------------------------------
float 
IvVector3::Length() const
{
    return IvSqrt( x*x + y*y + z*z );

}   // End of IvVector3::Length()


//-------------------------------------------------------------------------------
// @ IvVector3::LengthSquared()
//-------------------------------------------------------------------------------
// Vector length squared (avoids square root)
//-------------------------------------------------------------------------------
float 
IvVector3::LengthSquared() const
{
    return (x*x + y*y + z*z);

}   // End of IvVector3::LengthSquared()


//-------------------------------------------------------------------------------
// @ ::Distance()
//-------------------------------------------------------------------------------
// Point distance
//-------------------------------------------------------------------------------
float 
Distance( const IvVector3& p0, const IvVector3& p1 )
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;
    float z = p0.z - p1.z;

    return IvSqrt( x*x + y*y + z*z );

}   // End of IvVector3::Length()


//-------------------------------------------------------------------------------
// @ ::DistanceSquared()
//-------------------------------------------------------------------------------
// Point distance
//-------------------------------------------------------------------------------
float 
DistanceSquared( const IvVector3& p0, const IvVector3& p1 )
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;
    float z = p0.z - p1.z;

    return ( x*x + y*y + z*z );

}   // End of ::DistanceSquared()


//-------------------------------------------------------------------------------
// @ IvVector3::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvVector3::operator==( const IvVector3& other ) const
{
    if ( IvAreEqual( other.x, x )
        && IvAreEqual( other.y, y )
        && IvAreEqual( other.z, z ) )
        return true;

    return false;   
}   // End of IvVector3::operator==()


//-------------------------------------------------------------------------------
// @ IvVector3::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvVector3::operator!=( const IvVector3& other ) const
{
    if ( IvAreEqual( other.x, x )
        && IvAreEqual( other.y, y )
        && IvAreEqual( other.z, z ) )
        return false;

    return true;
}   // End of IvVector3::operator!=()


//-------------------------------------------------------------------------------
// @ IvVector3::IsZero()
//-------------------------------------------------------------------------------
// Check for zero vector
//-------------------------------------------------------------------------------
bool 
IvVector3::IsZero() const
{
    return IvIsZero(x*x + y*y + z*z);

}   // End of IvVector3::IsZero()


//-------------------------------------------------------------------------------
// @ IvVector3::IsUnit()
//-------------------------------------------------------------------------------
// Check for unit vector
//-------------------------------------------------------------------------------
bool 
IvVector3::IsUnit() const
{
    return IvIsZero(1.0f - x*x - y*y - z*z);

}   // End of IvVector3::IsUnit()


//-------------------------------------------------------------------------------
// @ IvVector3::Clean()
//-------------------------------------------------------------------------------
// Set elements close to zero equal to zero
//-------------------------------------------------------------------------------
void
IvVector3::Clean()
{
    if (IvIsZero(x))
    {
        x = 0.0f;
    }
    if (IvIsZero(y))
    {
        y = 0.0f;
    }
    if (IvIsZero(z))
    {
        z = 0.0f;
    }

}   // End of IvVector3::Clean()


//-------------------------------------------------------------------------------
// @ IvVector3::Normalize()
//-------------------------------------------------------------------------------
// Set to unit vector
//-------------------------------------------------------------------------------
void
IvVector3::Normalize()
{
    float lengthsq = x*x + y*y + z*z;

    if ( IvIsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = IvRecipSqrt( lengthsq );
        x *= factor;
        y *= factor;
        z *= factor;
    }

}   // End of IvVector3::Normalize()


//-------------------------------------------------------------------------------
// @ IvVector3::operator+()
//-------------------------------------------------------------------------------
// Add vector to self and return
//-------------------------------------------------------------------------------
IvVector3 
IvVector3::operator+( const IvVector3& other ) const
{
    return IvVector3( x + other.x, y + other.y, z + other.z );

}   // End of IvVector3::operator+()


//-------------------------------------------------------------------------------
// @ IvVector3::operator+=()
//-------------------------------------------------------------------------------
// Add vector to self, store in self
//-------------------------------------------------------------------------------
IvVector3& 
operator+=( IvVector3& self, const IvVector3& other )
{
    self.x += other.x;
    self.y += other.y;
    self.z += other.z;

    return self;

}   // End of IvVector3::operator+=()


//-------------------------------------------------------------------------------
// @ IvVector3::operator-()
//-------------------------------------------------------------------------------
// Subtract vector from self and return
//-------------------------------------------------------------------------------
IvVector3 
IvVector3::operator-( const IvVector3& other ) const
{
    return IvVector3( x - other.x, y - other.y, z - other.z );

}   // End of IvVector3::operator-()


//-------------------------------------------------------------------------------
// @ IvVector3::operator-=()
//-------------------------------------------------------------------------------
// Subtract vector from self, store in self
//-------------------------------------------------------------------------------
IvVector3& 
operator-=( IvVector3& self, const IvVector3& other )
{
    self.x -= other.x;
    self.y -= other.y;
    self.z -= other.z;

    return self;

}   // End of IvVector3::operator-=()

//-------------------------------------------------------------------------------
// @ IvVector3::operator-=() (unary)
//-------------------------------------------------------------------------------
// Negate self and return
//-------------------------------------------------------------------------------
IvVector3
IvVector3::operator-() const
{
    return IvVector3(-x, -y, -z);
}    // End of IvVector3::operator-()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
IvVector3   
IvVector3::operator*( float scalar )
{
    return IvVector3( scalar*x, scalar*y, scalar*z );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
IvVector3   
operator*( float scalar, const IvVector3& vector )
{
    return IvVector3( scalar*vector.x, scalar*vector.y, scalar*vector.z );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ IvVector3::operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication by self
//-------------------------------------------------------------------------------
IvVector3&
IvVector3::operator*=( float scalar )
{
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;

}   // End of IvVector3::operator*=()


//-------------------------------------------------------------------------------
// @ operator/()
//-------------------------------------------------------------------------------
// Scalar division
//-------------------------------------------------------------------------------
IvVector3   
IvVector3::operator/( float scalar )
{
    return IvVector3( x/scalar, y/scalar, z/scalar );

}   // End of operator/()


//-------------------------------------------------------------------------------
// @ IvVector3::operator/=()
//-------------------------------------------------------------------------------
// Scalar division by self
//-------------------------------------------------------------------------------
IvVector3&
IvVector3::operator/=( float scalar )
{
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;

}   // End of IvVector3::operator/=()


//-------------------------------------------------------------------------------
// @ IvVector3::Dot()
//-------------------------------------------------------------------------------
// Dot product by self
//-------------------------------------------------------------------------------
float               
IvVector3::Dot( const IvVector3& vector ) const
{
    return (x*vector.x + y*vector.y + z*vector.z);

}   // End of IvVector3::Dot()


//-------------------------------------------------------------------------------
// @ Dot()
//-------------------------------------------------------------------------------
// Dot product friend operator
//-------------------------------------------------------------------------------
float               
Dot( const IvVector3& vector1, const IvVector3& vector2 )
{
    return (vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z);

}   // End of Dot()


//-------------------------------------------------------------------------------
// @ IvVector3::Cross()
//-------------------------------------------------------------------------------
// Cross product by self
//-------------------------------------------------------------------------------
IvVector3   
IvVector3::Cross( const IvVector3& vector ) const
{
    return IvVector3( y*vector.z - z*vector.y,
                      z*vector.x - x*vector.z,
                      x*vector.y - y*vector.x );

}   // End of IvVector3::Cross()


//-------------------------------------------------------------------------------
// @ Cross()
//-------------------------------------------------------------------------------
// Cross product friend operator
//-------------------------------------------------------------------------------
IvVector3               
Cross( const IvVector3& vector1, const IvVector3& vector2 )
{
    return IvVector3( vector1.y*vector2.z - vector1.z*vector2.y,
                      vector1.z*vector2.x - vector1.x*vector2.z,
                      vector1.x*vector2.y - vector1.y*vector2.x );

}   // End of Cross()


