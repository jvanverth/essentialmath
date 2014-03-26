//----------------------------------------------------------------------------
// @ IvLine3.cpp
// 
// 3D line class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//----------------------------------------------------------------------------
//-- Includes ----------------------------------------------------------------
//----------------------------------------------------------------------------

#include "IvLine3.h"
#include "IvMath.h"
#include "IvMatrix44.h"
#include "IvQuat.h"
#include "IvVector3.h"

//----------------------------------------------------------------------------
//-- Static Variables --------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//-- Functions ---------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// @ IvLine3::IvLine3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvLine3::IvLine3() :
    mOrigin( 0.0f, 0.0f, 0.0f ),
    mDirection( 1.0f, 0.0f, 0.0f )
{
}   // End of IvLine3::IvLine3()


//----------------------------------------------------------------------------
// @ IvLine3::IvLine3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvLine3::IvLine3( const IvVector3& origin, const IvVector3& direction ) :
    mOrigin( origin ),
    mDirection( direction )
{
    mDirection.Normalize();

}   // End of IvLine3::IvLine3()


//----------------------------------------------------------------------------
// @ IvLine3::IvLine3()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
IvLine3::IvLine3( const IvLine3& other ) :
    mOrigin( other.mOrigin ),
    mDirection( other.mDirection )
{
    mDirection.Normalize();

}   // End of IvLine3::IvLine3()


//----------------------------------------------------------------------------
// @ IvLine3::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
IvLine3&
IvLine3::operator=( const IvLine3& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mOrigin = other.mOrigin;
    mDirection = other.mDirection;
    mDirection.Normalize();

    return *this;

}   // End of IvLine3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text IvLine3.  The format is assumed to be :
// [ IvVector3, IvVector3 ]
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvLine3& source)
{
    return out << "[" << source.GetOrigin() 
                     << ", " << source.GetDirection() << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ IvLine3::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
IvLine3::Get( IvVector3& origin, IvVector3& direction ) const
{
    origin = mOrigin;
    direction = mDirection;

}   // End of IvLine3::Get()


//----------------------------------------------------------------------------
// @ IvLine3::operator==()
// ---------------------------------------------------------------------------
// Are two IvLine3's equal?
//----------------------------------------------------------------------------
bool
IvLine3::operator==( const IvLine3& ray ) const
{
    return (ray.mOrigin == mOrigin && ray.mDirection == mDirection);

}  // End of IvLine3::operator==()


//----------------------------------------------------------------------------
// @ IvLine3::operator!=()
// ---------------------------------------------------------------------------
// Are two IvLine3's not equal?
//----------------------------------------------------------------------------
bool
IvLine3::operator!=( const IvLine3& ray ) const
{
    return !(ray.mOrigin == mOrigin && ray.mDirection == mDirection);
}  // End of IvLine3::operator!=()


//----------------------------------------------------------------------------
// @ IvLine3::Set()
// ---------------------------------------------------------------------------
// Sets the two parameters
//-----------------------------------------------------------------------------
void
IvLine3::Set( const IvVector3& origin, const IvVector3& direction )
{
    mOrigin = origin;
    mDirection = direction;
    mDirection.Normalize();

}   // End of IvLine3::Set()


//----------------------------------------------------------------------------
// @ IvLine3::Transform()
// ---------------------------------------------------------------------------
// Transforms ray into new space
//-----------------------------------------------------------------------------
IvLine3 
IvLine3::Transform( float scale, const IvQuat& rotate, const IvVector3& translate ) const
{
    IvLine3 line;
    IvMatrix44    transform(rotate);
    transform(0,0) *= scale;
    transform(1,0) *= scale;
    transform(2,0) *= scale;
    transform(0,1) *= scale;
    transform(1,1) *= scale;
    transform(2,1) *= scale;
    transform(0,2) *= scale;
    transform(1,2) *= scale;
    transform(2,2) *= scale;

    line.mDirection = transform.Transform( mDirection );
    line.mDirection.Normalize();

    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;

    line.mOrigin = transform.Transform( mOrigin );

    return line;

}   // End of IvLine3::Transform()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between lines.
//-----------------------------------------------------------------------------
float DistanceSquared( const IvLine3& line0, const IvLine3& line1, 
                       float& s_c, float& t_c )
{
    IvVector3 w0 = line0.mOrigin - line1.mOrigin;
    float a = line0.mDirection.Dot( line0.mDirection );
    float b = line0.mDirection.Dot( line1.mDirection );
    float c = line1.mDirection.Dot( line1.mDirection );
    float d = line0.mDirection.Dot( w0 );
    float e = line1.mDirection.Dot( w0 );
    float denom = a*c - b*b;
    if ( ::IsZero(denom) )
    {
        s_c = 0.0f;
        t_c = e/c;
        IvVector3 wc = w0 - t_c*line1.mDirection;
        return wc.Dot(wc);
    }
    else
    {
        s_c = ((b*e - c*d)/denom);
        t_c = ((a*e - b*d)/denom);
        IvVector3 wc = w0 + s_c*line0.mDirection
                          - t_c*line1.mDirection;
        return wc.Dot(wc);
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between line and point.
//-----------------------------------------------------------------------------
float DistanceSquared( const IvLine3& line, const IvVector3& point, float& t_c )
{
    IvVector3 w = point - line.mOrigin;
    float vsq = line.mDirection.Dot(line.mDirection);
    float proj = w.Dot(line.mDirection);
    t_c = proj/vsq; 

    return w.Dot(w) - t_c*proj;

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between two lines
//-----------------------------------------------------------------------------
void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                    const IvLine3& line0, 
                    const IvLine3& line1 )
{
    // compute intermediate parameters
    IvVector3 w0 = line0.mOrigin - line1.mOrigin;
    float a = line0.mDirection.Dot( line0.mDirection );
    float b = line0.mDirection.Dot( line1.mDirection );
    float c = line1.mDirection.Dot( line1.mDirection );
    float d = line0.mDirection.Dot( w0 );
    float e = line1.mDirection.Dot( w0 );

    float denom = a*c - b*b;

    if ( ::IsZero(denom) )
    {
        point0 = line0.mOrigin;
        point1 = line1.mOrigin + (e/c)*line1.mDirection;
    }
    else
    {
        point0 = line0.mOrigin + ((b*e - c*d)/denom)*line0.mDirection;
        point1 = line1.mOrigin + ((a*e - b*d)/denom)*line1.mDirection;
    }

}   // End of ClosestPoints()


//----------------------------------------------------------------------------
// @ IvLine3::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on line to point.
//-----------------------------------------------------------------------------
IvVector3 IvLine3::ClosestPoint(const IvVector3& point) const
{
    IvVector3 w = point - mOrigin;
    float vsq = mDirection.Dot(mDirection);
    float proj = w.Dot(mDirection);

    return mOrigin + (proj/vsq)*mDirection;

}   // End of IvLine3::ClosestPoint()
