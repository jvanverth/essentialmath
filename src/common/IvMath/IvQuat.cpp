//===============================================================================
// @ IvQuat.cpp
// 
// 3D quat class
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

#include "IvQuat.h"
#include "IvMath.h"
#include "IvVector3.h"
#include "IvMatrix33.h"

#include "IvAssert.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

IvQuat IvQuat::zero( 0.0f, 0.0f, 0.0f, 0.0f );
IvQuat IvQuat::identity( 1.0f, 0.0f, 0.0f, 0.0f );

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvQuat::IvQuat()
//-------------------------------------------------------------------------------
// Axis-angle constructor
//-------------------------------------------------------------------------------
IvQuat::IvQuat( const IvVector3& axis, float angle )
{
    Set( axis, angle );
}   // End of IvQuat::IvQuat()


//-------------------------------------------------------------------------------
// @ IvQuat::IvQuat()
//-------------------------------------------------------------------------------
// To-from vector constructor
//-------------------------------------------------------------------------------
IvQuat::IvQuat( const IvVector3& from, const IvVector3& to )
{
    Set( from, to );
}   // End of IvQuat::IvQuat()

//-------------------------------------------------------------------------------
// @ IvQuat::IvQuat()
//-------------------------------------------------------------------------------
// Vector constructor
//-------------------------------------------------------------------------------
IvQuat::IvQuat( const IvVector3& vector )
{
    Set( 0.0f, vector.x, vector.y, vector.z );
}   // End of IvQuat::IvQuat()


//-------------------------------------------------------------------------------
// @ IvQuat::IvQuat()
//-------------------------------------------------------------------------------
// Rotation matrix constructor
//-------------------------------------------------------------------------------
IvQuat::IvQuat( const IvMatrix33& rotation )
{
    float trace = rotation.Trace();
    if ( trace > 0.0f )
    {
        float s = ::IvSqrt( trace + 1.0f );
        w = s*0.5f;
        float recip = 0.5f/s;
        x = (rotation(2,1) - rotation(1,2))*recip;
        y = (rotation(0,2) - rotation(2,0))*recip;
        z = (rotation(1,0) - rotation(0,1))*recip;
    }
    else
    {
        unsigned int i = 0;
        if ( rotation(1,1) > rotation(0,0) )
            i = 1;
        if ( rotation(2,2) > rotation(i,i) )
            i = 2;
        unsigned int j = (i+1)%3;
        unsigned int k = (j+1)%3;
        float s = ::IvSqrt( rotation(i,i) - rotation(j,j) - rotation(k,k) + 1.0f );
        (*this)[i] = 0.5f*s;
        float recip = 0.5f/s;
        w = (rotation(k,j) - rotation(j,k))*recip;
        (*this)[j] = (rotation(j,i) + rotation(i,j))*recip;
        (*this)[k] = (rotation(k,i) + rotation(i,k))*recip;
    }

}   // End of IvQuat::IvQuat()


//-------------------------------------------------------------------------------
// @ IvQuat::IvQuat()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvQuat::IvQuat(const IvQuat& other) : 
    w( other.w ),
    x( other.x ),
    y( other.y ),
    z( other.z )
{

}   // End of IvQuat::IvQuat()


//-------------------------------------------------------------------------------
// @ IvQuat::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvQuat&
IvQuat::operator=(const IvQuat& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    w = other.w;
    x = other.x;
    y = other.y;
    z = other.z;
    
    return *this;

}   // End of IvQuat::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvQuat& source)
{
    out << '[' << source.w << ',' << source.x << ',' 
        << source.y << ',' << source.z << ']';

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvQuat::Magnitude()
//-------------------------------------------------------------------------------
// Quaternion magnitude (square root of norm)
//-------------------------------------------------------------------------------
float 
IvQuat::Magnitude() const
{
    return IvSqrt( w*w + x*x + y*y + z*z );

}   // End of IvQuat::Magnitude()


//-------------------------------------------------------------------------------
// @ IvQuat::Norm()
//-------------------------------------------------------------------------------
// Quaternion norm
//-------------------------------------------------------------------------------
float 
IvQuat::Norm() const
{
    return ( w*w + x*x + y*y + z*z );

}   // End of IvQuat::Norm()


//-------------------------------------------------------------------------------
// @ IvQuat::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvQuat::operator==( const IvQuat& other ) const
{
    if ( IvIsZero( other.w - w )
        && IvIsZero( other.x - x )
        && IvIsZero( other.y - y )
        && IvIsZero( other.z - z ) )
        return true;

    return false;   
}   // End of IvQuat::operator==()


//-------------------------------------------------------------------------------
// @ IvQuat::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvQuat::operator!=( const IvQuat& other ) const
{
    if ( IvIsZero( other.w - w )
        || IvIsZero( other.x - x )
        || IvIsZero( other.y - y )
        || IvIsZero( other.z - z ) )
        return false;

    return true;
}   // End of IvQuat::operator!=()


//-------------------------------------------------------------------------------
// @ IvQuat::IsZero()
//-------------------------------------------------------------------------------
// Check for zero quat
//-------------------------------------------------------------------------------
bool 
IvQuat::IsZero() const
{
    return IvIsZero(w*w + x*x + y*y + z*z);

}   // End of IvQuat::IsZero()


//-------------------------------------------------------------------------------
// @ IvQuat::IsUnit()
//-------------------------------------------------------------------------------
// Check for unit quat
//-------------------------------------------------------------------------------
bool 
IvQuat::IsUnit() const
{
    return IvIsZero(1.0f - w*w - x*x - y*y - z*z);

}   // End of IvQuat::IsUnit()


//-------------------------------------------------------------------------------
// @ IvQuat::IsIdentity()
//-------------------------------------------------------------------------------
// Check for identity quat
//-------------------------------------------------------------------------------
bool 
IvQuat::IsIdentity() const
{
    return IvIsZero(1.0f - w)
        && IvIsZero( x ) 
        && IvIsZero( y )
        && IvIsZero( z );

}   // End of IvQuat::IsIdentity()


//-------------------------------------------------------------------------------
// @ IvQuat::Set()
//-------------------------------------------------------------------------------
// Set quaternion based on axis-angle
//-------------------------------------------------------------------------------
void
IvQuat::Set( const IvVector3& axis, float angle )
{
    // if axis of rotation is zero vector, just set to identity quat
    float length = axis.LengthSquared();
    if ( IvIsZero( length ) )
    {
        Identity();
        return;
    }

    // take half-angle
    angle *= 0.5f;

    float sintheta, costheta;
    IvSinCos(angle, sintheta, costheta);

    float scaleFactor = sintheta/IvSqrt( length );

    w = costheta;
    x = scaleFactor * axis.x;
    y = scaleFactor * axis.y;
    z = scaleFactor * axis.z;

}   // End of IvQuat::Set()


//-------------------------------------------------------------------------------
// @ IvQuat::Set()
//-------------------------------------------------------------------------------
// Set quaternion based on start and end vectors
// 
// This is a slightly faster method than that presented in the book, and it 
// doesn't require unit vectors as input.  Found on GameDev.net, in an article by
// minorlogic.  Original source unknown.
//-------------------------------------------------------------------------------
void
IvQuat::Set( const IvVector3& from, const IvVector3& to )
{
   // get axis of rotation
    IvVector3 axis = from.Cross( to );

    // get scaled cos of angle between vectors and set initial quaternion
    Set(  from.Dot( to ), axis.x, axis.y, axis.z );
    // quaternion at this point is ||from||*||to||*( cos(theta), r*sin(theta) )

    // normalize to remove ||from||*||to|| factor
    Normalize();
    // quaternion at this point is ( cos(theta), r*sin(theta) )
    // what we want is ( cos(theta/2), r*sin(theta/2) )

    // set up for half angle calculation
    w += 1.0f;

    // now when we normalize, we'll be dividing by sqrt(2*(1+cos(theta))), which is 
    // what we want for r*sin(theta) to give us r*sin(theta/2)  (see pages 487-488)
    // 
    // w will become 
    //                 1+cos(theta)
    //            ----------------------
    //            sqrt(2*(1+cos(theta)))        
    // which simplifies to
    //                cos(theta/2)

    // before we normalize, check if vectors are opposing
    if ( w <= kEpsilon )
    {
        // rotate pi radians around orthogonal vector
        // take cross product with x axis
        if ( from.z*from.z > from.x*from.x )
            Set( 0.0f, 0.0f, from.z, -from.y );
        // or take cross product with z axis
        else
            Set( 0.0f, from.y, -from.x, 0.0f );
    }
   
    // normalize again to get rotation quaternion
    Normalize();

}   // End of IvQuat::Set()


//-------------------------------------------------------------------------------
// @ IvQuat::Set()
//-------------------------------------------------------------------------------
// Set quaternion based on fixed angles
//-------------------------------------------------------------------------------
void 
IvQuat::Set( float zRotation, float yRotation, float xRotation ) 
{
    zRotation *= 0.5f;
    yRotation *= 0.5f;
    xRotation *= 0.5f;

    // get sines and cosines of half angles
    float Cx, Sx;
    IvSinCos(xRotation, Sx, Cx);

    float Cy, Sy;
    IvSinCos(yRotation, Sy, Cy);

    float Cz, Sz;
    IvSinCos(zRotation, Sz, Cz);

    // multiply it out
    w = Cx*Cy*Cz - Sx*Sy*Sz;
    x = Sx*Cy*Cz + Cx*Sy*Sz;
    y = Cx*Sy*Cz - Sx*Cy*Sz;
    z = Cx*Cy*Sz + Sx*Sy*Cx;

}   // End of IvQuat::Set()


//-------------------------------------------------------------------------------
// @ IvQuat::GetAxisAngle()
//-------------------------------------------------------------------------------
// Get axis-angle based on quaternion
//-------------------------------------------------------------------------------
void
IvQuat::GetAxisAngle( IvVector3& axis, float& angle )
{
    angle = 2.0f*acosf( w );
    float length = ::IvSqrt( 1.0f - w*w );
    if ( IvIsZero(length) )
        axis.Zero();
    else
    {
        length = 1.0f/length;
        axis.Set( x*length, y*length, z*length );
    }

}   // End of IvQuat::GetAxisAngle()


//-------------------------------------------------------------------------------
// @ IvQuat::Clean()
//-------------------------------------------------------------------------------
// Set elements close to zero equal to zero
//-------------------------------------------------------------------------------
void
IvQuat::Clean()
{
    if (IvIsZero(w))
    {
        w = 0.0f;
    }
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

}   // End of IvQuat::Clean()


//-------------------------------------------------------------------------------
// @ IvQuat::Normalize()
//-------------------------------------------------------------------------------
// Set to unit quaternion
//-------------------------------------------------------------------------------
void
IvQuat::Normalize()
{
    float lengthsq = w*w + x*x + y*y + z*z;

    if ( IvIsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = IvRecipSqrt( lengthsq );
        w *= factor;
        x *= factor;
        y *= factor;
        z *= factor;
    }

}   // End of IvQuat::Normalize()


//-------------------------------------------------------------------------------
// @ ::Conjugate()
//-------------------------------------------------------------------------------
// Compute complex conjugate
//-------------------------------------------------------------------------------
IvQuat 
Conjugate( const IvQuat& quat ) 
{
    return IvQuat( quat.w, -quat.x, -quat.y, -quat.z );

}   // End of Conjugate()


//-------------------------------------------------------------------------------
// @ IvQuat::Conjugate()
//-------------------------------------------------------------------------------
// Set self to complex conjugate
//-------------------------------------------------------------------------------
const IvQuat& 
IvQuat::Conjugate()
{
    x = -x;
    y = -y;
    z = -z;

    return *this;

}   // End of Conjugate()


//-------------------------------------------------------------------------------
// @ ::Inverse()
//-------------------------------------------------------------------------------
// Compute quaternion inverse
//-------------------------------------------------------------------------------
IvQuat 
Inverse( const IvQuat& quat )
{
    float norm = quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;
    // if we're the zero quaternion, just return identity
    if ( !IvIsZero( norm ) )
    {
        ASSERT( false );
        return IvQuat();
    }

    float normRecip = 1.0f / norm;
    return IvQuat( normRecip*quat.w, -normRecip*quat.x, -normRecip*quat.y, 
                   -normRecip*quat.z );

}   // End of Inverse()


//-------------------------------------------------------------------------------
// @ IvQuat::Inverse()
//-------------------------------------------------------------------------------
// Set self to inverse
//-------------------------------------------------------------------------------
const IvQuat& 
IvQuat::Inverse()
{
    float norm = w*w + x*x + y*y + z*z;
    // if we're the zero quaternion, just return
    if (IvIsZero(norm))
    {
        return *this;
    }

    float normRecip = 1.0f / norm;
    w = normRecip*w;
    x = -normRecip*x;
    y = -normRecip*y;
    z = -normRecip*z;

    return *this;

}   // End of Inverse()


//-------------------------------------------------------------------------------
// @ IvQuat::operator+()
//-------------------------------------------------------------------------------
// Add quat to self and return
//-------------------------------------------------------------------------------
IvQuat 
IvQuat::operator+( const IvQuat& other ) const
{
    return IvQuat( w + other.w, x + other.x, y + other.y, z + other.z );

}   // End of IvQuat::operator+()


//-------------------------------------------------------------------------------
// @ IvQuat::operator+=()
//-------------------------------------------------------------------------------
// Add quat to self, store in self
//-------------------------------------------------------------------------------
IvQuat& 
IvQuat::operator+=( const IvQuat& other )
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;

}   // End of IvQuat::operator+=()


//-------------------------------------------------------------------------------
// @ IvQuat::operator-()
//-------------------------------------------------------------------------------
// Subtract quat from self and return
//-------------------------------------------------------------------------------
IvQuat 
IvQuat::operator-( const IvQuat& other ) const
{
    return IvQuat( w - other.w, x - other.x, y - other.y, z - other.z );

}   // End of IvQuat::operator-()


//-------------------------------------------------------------------------------
// @ IvQuat::operator-=()
//-------------------------------------------------------------------------------
// Subtract quat from self, store in self
//-------------------------------------------------------------------------------
IvQuat& 
IvQuat::operator-=( const IvQuat& other )
{
    w -= other.w;
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;

}   // End of IvQuat::operator-=()


//-------------------------------------------------------------------------------
// @ IvQuat::operator-=() (unary)
//-------------------------------------------------------------------------------
// Negate self and return
//-------------------------------------------------------------------------------
IvQuat
IvQuat::operator-() const
{
    return IvQuat(-w, -x, -y, -z);
}    // End of IvQuat::operator-()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
IvQuat  
operator*( float scalar, const IvQuat& quat )
{
    return IvQuat( scalar*quat.w, scalar*quat.x, scalar*quat.y, scalar*quat.z );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ IvQuat::operator*=()
//-------------------------------------------------------------------------------
// Scalar multiplication by self
//-------------------------------------------------------------------------------
IvQuat&
IvQuat::operator*=( float scalar )
{
    w *= scalar;
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;

}   // End of IvQuat::operator*=()


//-------------------------------------------------------------------------------
// @ IvQuat::operator*()
//-------------------------------------------------------------------------------
// Quaternion multiplication
//-------------------------------------------------------------------------------
IvQuat  
IvQuat::operator*( const IvQuat& other ) const
{
    return IvQuat( w*other.w - x*other.x - y*other.y - z*other.z,
                   w*other.x + x*other.w + y*other.z - z*other.y,
                   w*other.y + y*other.w + z*other.x - x*other.z,
                   w*other.z + z*other.w + x*other.y - y*other.x );

}   // End of IvQuat::operator*()


//-------------------------------------------------------------------------------
// @ IvQuat::operator*=()
//-------------------------------------------------------------------------------
// Quaternion multiplication by self
//-------------------------------------------------------------------------------
IvQuat&
IvQuat::operator*=( const IvQuat& other )
{
    Set( w*other.w - x*other.x - y*other.y - z*other.z,
         w*other.x + x*other.w + y*other.z - z*other.y,
         w*other.y + y*other.w + z*other.x - x*other.z,
         w*other.z + z*other.w + x*other.y - y*other.x );
  
    return *this;

}   // End of IvQuat::operator*=()


//-------------------------------------------------------------------------------
// @ IvQuat::Dot()
//-------------------------------------------------------------------------------
// Dot product by self
//-------------------------------------------------------------------------------
float               
IvQuat::Dot( const IvQuat& quat ) const
{
    return ( w*quat.w + x*quat.x + y*quat.y + z*quat.z);

}   // End of IvQuat::Dot()


//-------------------------------------------------------------------------------
// @ Dot()
//-------------------------------------------------------------------------------
// Dot product friend operator
//-------------------------------------------------------------------------------
float               
Dot( const IvQuat& quat1, const IvQuat& quat2 )
{
    return (quat1.w*quat2.w + quat1.x*quat2.x + quat1.y*quat2.y + quat1.z*quat2.z);

}   // End of Dot()


//-------------------------------------------------------------------------------
// @ IvQuat::Rotate()
//-------------------------------------------------------------------------------
// Rotate vector by quaternion
// Assumes quaternion is normalized!
//-------------------------------------------------------------------------------
IvVector3   
IvQuat::Rotate( const IvVector3& vector ) const
{
    ASSERT( IsUnit() );

    float vMult = 2.0f*(x*vector.x + y*vector.y + z*vector.z);
    float crossMult = 2.0f*w;
    float pMult = crossMult*w - 1.0f;

    return IvVector3( pMult*vector.x + vMult*x + crossMult*(y*vector.z - z*vector.y),
                      pMult*vector.y + vMult*y + crossMult*(z*vector.x - x*vector.z),
                      pMult*vector.z + vMult*z + crossMult*(x*vector.y - y*vector.x) );

}   // End of IvQuat::Rotate()


//-------------------------------------------------------------------------------
// @ Lerp()
//-------------------------------------------------------------------------------
// Linearly interpolate two quaternions
// This will always take the shorter path between them
//-------------------------------------------------------------------------------
void 
Lerp( IvQuat& result, const IvQuat& start, const IvQuat& end, float t )
{
    // get cos of "angle" between quaternions
    float cosTheta = start.Dot( end );

    // initialize result
    result = t*end;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= kEpsilon )
    {
        // use standard interpolation
        result += (1.0f-t)*start;
    }
    else
    {
        // otherwise, take the shorter path
        result += (t-1.0f)*start;
    }

}   // End of Lerp()


//-------------------------------------------------------------------------------
// @ Slerp()
//-------------------------------------------------------------------------------
// Spherical linearly interpolate two quaternions
// This will always take the shorter path between them
//-------------------------------------------------------------------------------
void 
Slerp( IvQuat& result, const IvQuat& start, const IvQuat& end, float t )
{
    // get cosine of "angle" between quaternions
    float cosTheta = start.Dot( end );
    float startInterp, endInterp;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= kEpsilon )
    {
        // if angle is greater than zero
        if ( (1.0f - cosTheta) > kEpsilon )
        {
            // use standard slerp
            float theta = acosf( cosTheta );
            float recipSinTheta = 1.0f/IvSin( theta );

            startInterp = IvSin( (1.0f - t)*theta )*recipSinTheta;
            endInterp = IvSin( t*theta )*recipSinTheta;
        }
        // angle is close to zero
        else
        {
            // use linear interpolation
            startInterp = 1.0f - t;
            endInterp = t;
        }
    }
    // otherwise, take the shorter route
    else
    {
        // if angle is less than 180 degrees
        if ( (1.0f + cosTheta) > kEpsilon )
        {
            // use slerp w/negation of start quaternion
            float theta = acosf( -cosTheta );
            float recipSinTheta = 1.0f/IvSin( theta );

            startInterp = IvSin( (t-1.0f)*theta )*recipSinTheta;
            endInterp = IvSin( t*theta )*recipSinTheta;
        }
        // angle is close to 180 degrees
        else
        {
            // use lerp w/negation of start quaternion
            startInterp = t - 1.0f;
            endInterp = t;
        }
    }
    
    result = startInterp*start + endInterp*end;

}   // End of Slerp()


//-------------------------------------------------------------------------------
// @ ApproxSlerp()
//-------------------------------------------------------------------------------
// Approximate spherical linear interpolation of two quaternions
// Based on "Hacking Quaternions", Jonathan Blow, Game Developer, March 2002.
// See Game Developer, February 2004 for an alternate method.
//-------------------------------------------------------------------------------
void 
ApproxSlerp( IvQuat& result, const IvQuat& start, const IvQuat& end, float t )
{
    float cosTheta = start.Dot( end );

    // correct time by using cosine of angle between quaternions
    float factor = 1.0f - 0.7878088f*cosTheta;
    float k = 0.5069269f;
    factor *= factor;
    k *= factor;

    float b = 2*k;
    float c = -3*k;
    float d = 1 + k;

    t = t*(b*t + c) + d;

    // initialize result
    result = t*end;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= kEpsilon )
    {
        // use standard interpolation
        result += (1.0f-t)*start;
    }
    else
    {
        // otherwise, take the shorter path
        result += (t-1.0f)*start;
    }

}   // End of ApproxSlerp()
