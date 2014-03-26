//----------------------------------------------------------------------------
// @ IvRay3.cpp
// 
// 3D ray class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//----------------------------------------------------------------------------
//-- Includes ----------------------------------------------------------------
//----------------------------------------------------------------------------

#include "IvRay3.h"
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
// @ IvRay3::IvRay3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvRay3::IvRay3() :
    mOrigin( 0.0f, 0.0f, 0.0f ),
    mDirection( 1.0f, 0.0f, 0.0f )
{
}   // End of IvRay3::IvRay3()


//----------------------------------------------------------------------------
// @ IvRay3::IvRay3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvRay3::IvRay3( const IvVector3& origin, const IvVector3& direction ) :
    mOrigin( origin ),
    mDirection( direction )
{
    mDirection.Normalize();

}   // End of IvRay3::IvRay3()


//----------------------------------------------------------------------------
// @ IvRay3::IvRay3()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
IvRay3::IvRay3( const IvRay3& other ) :
    mOrigin( other.mOrigin ),
    mDirection( other.mDirection )
{
    mDirection.Normalize();

}   // End of IvRay3::IvRay3()


//----------------------------------------------------------------------------
// @ IvRay3::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
IvRay3&
IvRay3::operator=( const IvRay3& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mOrigin = other.mOrigin;
    mDirection = other.mDirection;
    mDirection.Normalize();

    return *this;

}   // End of IvRay3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text IvRay3.  The format is assumed to be :
// [ IvVector3, IvVector3 ]
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvRay3& source)
{
    return out << "[" << source.GetOrigin() 
                     << ", " << source.GetDirection() << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ IvRay3::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
IvRay3::Get( IvVector3& origin, IvVector3& direction ) const
{
    origin = mOrigin;
    direction = mDirection;

}   // End of IvRay3::Get()


//----------------------------------------------------------------------------
// @ IvRay3::operator==()
// ---------------------------------------------------------------------------
// Are two IvRay3's equal?
//----------------------------------------------------------------------------
bool
IvRay3::operator==( const IvRay3& ray ) const
{
    return (ray.mOrigin == mOrigin && ray.mDirection == mDirection);

}  // End of IvRay3::operator==()


//----------------------------------------------------------------------------
// @ IvRay3::operator!=()
// ---------------------------------------------------------------------------
// Are two IvRay3's not equal?
//----------------------------------------------------------------------------
bool
IvRay3::operator!=( const IvRay3& ray ) const
{
    return !(ray.mOrigin == mOrigin && ray.mDirection == mDirection);
}  // End of IvRay3::operator!=()


//----------------------------------------------------------------------------
// @ IvRay3::Set()
// ---------------------------------------------------------------------------
// Sets the two parameters
//-----------------------------------------------------------------------------
void
IvRay3::Set( const IvVector3& origin, const IvVector3& direction )
{
    mOrigin = origin;
    mDirection = direction;
    mDirection.Normalize();

}   // End of IvRay3::Set()


//----------------------------------------------------------------------------
// @ IvRay3::Transform()
// ---------------------------------------------------------------------------
// Transforms ray into new space
//-----------------------------------------------------------------------------
IvRay3  
IvRay3::Transform( float scale, const IvQuat& rotate, const IvVector3& translate ) const
{
    IvRay3 ray;
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

    ray.mDirection = transform.Transform( mDirection );
    ray.mDirection.Normalize();

    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;

    ray.mOrigin = transform.Transform( mOrigin );

    return ray;

}   // End of IvRay3::Transform()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between rays.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const IvRay3& ray0, const IvRay3& ray1, 
                 float& s_c, float& t_c )
{
    // compute intermediate parameters
    IvVector3 w0 = ray0.mOrigin - ray1.mOrigin;
    float a = ray0.mDirection.Dot( ray0.mDirection );
    float b = ray0.mDirection.Dot( ray1.mDirection );
    float c = ray1.mDirection.Dot( ray1.mDirection );
    float d = ray0.mDirection.Dot( w0 );
    float e = ray1.mDirection.Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( ::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,+inf]
        sd = td = denom;
        sn = b*e - c*d;
        tn = a*e - b*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            sn = 0.0f;
            tn = e;
            td = c;
        }
    }

    // clamp t_c within [0,+inf]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute difference vector and distance squared
    IvVector3 wc = w0 + s_c*ray0.mDirection - t_c*ray1.mDirection;
    return wc.Dot(wc);

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ IvRay3::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between ray and line.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const IvRay3& ray, const IvLine3& line, 
                   float& s_c, float& t_c )
{
    // compute intermediate parameters
    IvVector3 w0 = ray.mOrigin - line.GetOrigin();
    float a = ray.mDirection.Dot( ray.mDirection );
    float b = ray.mDirection.Dot( line.GetDirection() );
    float c = line.GetDirection().Dot( line.GetDirection() );
    float d = ray.mDirection.Dot( w0 );
    float e = line.GetDirection().Dot( w0 );

    float denom = a*c - b*b;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( ::IsZero(denom) )
    {
        s_c = 0.0f;
        t_c = e/c;
        // compute difference vector and distance squared
        IvVector3 wc = w0 - t_c*line.GetDirection();
        return wc.Dot(wc);
    }
    else
    {
        // parameters to compute s_c, t_c
        float sn;

        // clamp s_c within [0,1]
        sn = b*e - c*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            s_c = 0.0f;
            t_c = e/c;
        }
        // clamp s_c to 1
        else if (sn > denom)
        {
            s_c = 1.0f;
            t_c = (e+b)/c;
        }
        else
        {
            s_c = sn/denom;
            t_c = (a*e - b*d)/denom;
        }

        // compute difference vector and distance squared
        IvVector3 wc = w0 + s_c*ray.mDirection - t_c*line.GetDirection();
        return wc.Dot(wc);
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between ray and point.
//-----------------------------------------------------------------------------
float DistanceSquared( const IvRay3& ray, const IvVector3& point, 
                       float& t_c ) 
{
    IvVector3 w = point - ray.mOrigin;
    float proj = w.Dot(ray.mDirection);
    // origin is closest point
    if ( proj <= 0 )
    {
        t_c = 0.0f;
        return w.Dot(w);
    }
    // else use normal line check
    else
    {
        float vsq = ray.mDirection.Dot(ray.mDirection);
        t_c = proj/vsq;
        return w.Dot(w) - t_c*proj;
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between two rays
//-----------------------------------------------------------------------------
void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                    const IvRay3& ray0, 
                    const IvRay3& ray1 )
{
    // compute intermediate parameters
    IvVector3 w0 = ray0.mOrigin - ray1.mOrigin;
    float a = ray0.mDirection.Dot( ray0.mDirection );
    float b = ray0.mDirection.Dot( ray1.mDirection );
    float c = ray1.mDirection.Dot( ray1.mDirection );
    float d = ray0.mDirection.Dot( w0 );
    float e = ray1.mDirection.Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float s_c, t_c;
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( ::IsZero(denom) )
    {
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // start by clamping s_c
        sd = td = denom;
        sn = b*e - c*d;
        tn = a*e - b*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            sn = 0.0f;
            tn = e;
            td = c;
        }
    }

    // clamp t_c within [0,+inf]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute closest points
    point0 = ray0.mOrigin + s_c*ray0.mDirection;
    point1 = ray1.mOrigin + t_c*ray1.mDirection;
}


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between ray and line
//-----------------------------------------------------------------------------
void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                    const IvRay3& ray, 
                    const IvLine3& line )
{
    // compute intermediate parameters
    IvVector3 w0 = ray.mOrigin - line.GetOrigin();
    float a = ray.mDirection.Dot( ray.mDirection );
    float b = ray.mDirection.Dot( line.GetDirection() );
    float c = line.GetDirection().Dot( line.GetDirection() );
    float d = ray.mDirection.Dot( w0 );
    float e = line.GetDirection().Dot( w0 );

    float denom = a*c - b*b;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( ::IsZero(denom) )
    {
        // compute closest points
        point0 = ray.mOrigin;
        point1 = line.GetOrigin() + (e/c)*line.GetDirection();
    }
    else
    {
        // parameters to compute s_c, t_c
        float sn, s_c, t_c;

        // clamp s_c within [0,1]
        sn = b*e - c*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            s_c = 0.0f;
            t_c = e/c;
        }
        // clamp s_c to 1
        else if (sn > denom)
        {
            s_c = 1.0f;
            t_c = (e+b)/c;
        }
        else
        {
            s_c = sn/denom;
            t_c = (a*e - b*d)/denom;
        }

        // compute closest points
        point0 = ray.mOrigin + s_c*ray.mDirection;
        point1 = line.GetOrigin() + t_c*line.GetDirection();
    }

}   // End of ClosestPoints()


//----------------------------------------------------------------------------
// @ IvRay3::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on ray to point
//-----------------------------------------------------------------------------
IvVector3 
IvRay3::ClosestPoint( const IvVector3& point ) const
{
    IvVector3 w = point - mOrigin;
    float proj = w.Dot(mDirection);
    // endpoint 0 is closest point
    if ( proj <= 0.0f )
        return mOrigin;
    else
    {
        float vsq = mDirection.Dot(mDirection);
        // else somewhere else in ray
        return mOrigin + (proj/vsq)*mDirection;
    }
}
