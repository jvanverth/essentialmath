//===============================================================================
// @ IvVector4.cpp
// 
// 3D vector class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVector4.h"
#include "IvMath.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

IvVector4 IvVector4::xAxis( 1.0f, 0.0f, 0.0f, 0.0f );
IvVector4 IvVector4::yAxis( 0.0f, 1.0f, 0.0f, 0.0f );
IvVector4 IvVector4::zAxis( 0.0f, 0.0f, 1.0f, 0.0f );
IvVector4 IvVector4::wAxis( 0.0f, 0.0f, 0.0f, 1.0f );
IvVector4 IvVector4::origin( 0.0f, 0.0f, 0.0f, 0.0f );

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVector4::IvVector4()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvVector4::IvVector4(const IvVector4& other) : 
    x( other.x ),
    y( other.y ),
    z( other.z ),
    w( other.w )
{

}   // End of IvVector4::IvVector4()


//-------------------------------------------------------------------------------
// @ IvVector4::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvVector4&
IvVector4::operator=(const IvVector4& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    
    return *this;

}   // End of IvVector4::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvVector4& source)
{
    out << '<' << source.x << ',' << source.y << ',' << source.z << ','
        << source.w << '>';

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvVector4::Length()
//-------------------------------------------------------------------------------
// Vector length
//-------------------------------------------------------------------------------
float 
IvVector4::Length() const
{
    return IvSqrt( x*x + y*y + z*z + w*w );

}   // End of IvVector4::Length()


//-------------------------------------------------------------------------------
// @ IvVector4::LengthSquared()
//-------------------------------------------------------------------------------
// Vector length squared (avoids square root)
//-------------------------------------------------------------------------------
float 
IvVector4::LengthSquared() const
{
    return ( x*x + y*y + z*z + w*w );

}   // End of IvVector4::LengthSquared()


//-------------------------------------------------------------------------------
// @ IvVector4::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvVector4::operator==( const IvVector4& other ) const
{
    if ( ::IvAreEqual( other.x, x )
        && ::IvAreEqual( other.y, y )
        && ::IvAreEqual( other.z, z )
        && ::IvAreEqual( other.w, w ) )
        return true;

    return false;   
}   // End of IvVector4::operator==()


//-------------------------------------------------------------------------------
// @ IvVector4::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvVector4::operator!=( const IvVector4& other ) const
{
    if ( ::IvAreEqual( other.x, x )
        && ::IvAreEqual( other.y, y )
        && ::IvAreEqual( other.z, z )
        && ::IvAreEqual( other.w, w ) )
        return false;

    return true;
}   // End of IvVector4::operator!=()


//-------------------------------------------------------------------------------
// @ IvVector4::IsZero()
//-------------------------------------------------------------------------------
// Check for zero vector
//-------------------------------------------------------------------------------
bool 
IvVector4::IsZero() const
{
    return ::IsZero(x*x + y*y + z*z + w*w);

}   // End of IvVector4::IsZero()


//-------------------------------------------------------------------------------
// @ IvVector4::IsUnit()
//-------------------------------------------------------------------------------
// Check for unit vector
//-------------------------------------------------------------------------------
bool 
IvVector4::IsUnit() const
{
    return ::IsZero(1.0f - x*x - y*y - z*z - w*w);

}   // End of IvVector4::IsUnit()


//-------------------------------------------------------------------------------
// @ IvVector4::Clean()
//-------------------------------------------------------------------------------
// Set elements close to zero equal to zero
//-------------------------------------------------------------------------------
void
IvVector4::Clean()
{
    if ( ::IsZero( x ) )
        x = 0.0f;
    if ( ::IsZero( y ) )
        y = 0.0f;
    if ( ::IsZero( z ) )
        z = 0.0f;
    if ( ::IsZero( w ) )
        w = 0.0f;

}   // End of IvVector4::Clean()


//-------------------------------------------------------------------------------
// @ IvVector4::Normalize()
//-------------------------------------------------------------------------------
// Set to unit vector
//-------------------------------------------------------------------------------
void
IvVector4::Normalize()
{
    float lengthsq = x*x + y*y + z*z + w*w;

    if ( ::IsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = IvInvSqrt( lengthsq );
        x *= factor;
        y *= factor;
        z *= factor;
        w *= factor;
    }

}   // End of IvVector4::Normalize()


//-------------------------------------------------------------------------------
// @ IvVector4::operator+()
//-------------------------------------------------------------------------------
// Add vector to self and return
//-------------------------------------------------------------------------------
IvVector4 
IvVector4::operator+( const IvVector4& other ) const
{
    return IvVector4( x + other.x, y + other.y, z + other.z, w + other.w );

}   // End of IvVector4::operator+()


//-------------------------------------------------------------------------------
// @ IvVector4::operator+=()
//-------------------------------------------------------------------------------
// Add vector to self, store in self
//-------------------------------------------------------------------------------
IvVector4& 
IvVector4::operator+=( const IvVector4& other )
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;

}   // End of IvVector4::operator+=()


//-------------------------------------------------------------------------------
// @ IvVector4::operator-()
//-------------------------------------------------------------------------------
// Subtract vector from self and return
//-------------------------------------------------------------------------------
IvVector4 
IvVector4::operator-( const IvVector4& other ) const
{
    return IvVector4( x - other.x, y - other.y, z - other.z, w - other.w );

}   // End of IvVector4::operator-()


//-------------------------------------------------------------------------------
// @ IvVector4::operator-=()
//-------------------------------------------------------------------------------
// Subtract vector from self, store in self
//-------------------------------------------------------------------------------
IvVector4& 
IvVector4::operator-=( const IvVector4& other )
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;

}   // End of IvVector4::operator-=()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
IvVector4   
IvVector4::operator*( float scalar )
{
    return IvVector4( scalar*x, scalar*y, scalar*z,
                      scalar*w );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
IvVector4   
operator*( float scalar, const IvVector4& vector )
{
    return IvVector4( scalar*vector.x, scalar*vector.y, scalar*vector.z,
                      scalar*vector.w );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ IvVector4::operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication by self
//-------------------------------------------------------------------------------
IvVector4&
IvVector4::operator*=( float scalar )
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;

}   // End of IvVector4::operator*()


//-------------------------------------------------------------------------------
// @ operator/()
//-------------------------------------------------------------------------------
// Scalar division
//-------------------------------------------------------------------------------
IvVector4   
IvVector4::operator/( float scalar )
{
    return IvVector4( x/scalar, y/scalar, z/scalar, w/scalar );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ IvVector4::operator/=()
//-------------------------------------------------------------------------------
// Scalar division by self
//-------------------------------------------------------------------------------
IvVector4&
IvVector4::operator/=( float scalar )
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;

    return *this;

}   // End of IvVector4::operator/=()


//-------------------------------------------------------------------------------
// @ IvVector4::Dot()
//-------------------------------------------------------------------------------
// Dot product by self
//-------------------------------------------------------------------------------
float               
IvVector4::Dot( const IvVector4& vector ) const
{
    return (x*vector.x + y*vector.y + z*vector.z + w*vector.w);

}   // End of IvVector4::Dot()


//-------------------------------------------------------------------------------
// @ Dot()
//-------------------------------------------------------------------------------
// Dot product friend operator
//-------------------------------------------------------------------------------
float               
Dot( const IvVector4& vector1, const IvVector4& vector2 )
{
    return (vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z
            + vector1.w*vector2.w);

}   // End of Dot()
