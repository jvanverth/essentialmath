//===============================================================================
// @ IvBoundingSphere.cpp
// 
// sphere collision class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvBoundingSphere.h"
#include "IvVector3.h"
#include "IvLine3.h"
#include "IvMatrix33.h"
#include "IvRay3.h"
#include "IvLineSegment3.h"
#include "IvMath.h"
#include "IvPlane.h"
#include "IvAssert.h"
#include "IvQuat.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvBoundingSphere::IvBoundingSphere()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvBoundingSphere::IvBoundingSphere(const IvBoundingSphere& other) : 
    mCenter( other.mCenter ),
    mRadius( other.mRadius )
{

}   // End of IvBoundingSphere::IvBoundingSphere()


//-------------------------------------------------------------------------------
// @ IvBoundingSphere::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvBoundingSphere&
IvBoundingSphere::operator=(const IvBoundingSphere& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mCenter = other.mCenter;
    mRadius = other.mRadius;
    
    return *this;

}   // End of IvBoundingSphere::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvBoundingSphere& source)
{
    out << source.mCenter;
    out << ' ' << source.mRadius;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvBoundingSphere::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvBoundingSphere::operator==( const IvBoundingSphere& other ) const
{
    if ( other.mCenter == mCenter && other.mRadius == mRadius )
        return true;

    return false;   
}   // End of IvBoundingSphere::operator==()


//-------------------------------------------------------------------------------
// @ IvBoundingSphere::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvBoundingSphere::operator!=( const IvBoundingSphere& other ) const
{
    if ( other.mCenter != mCenter || other.mRadius != mRadius )
        return true;

    return false;

}   // End of IvBoundingSphere::operator!=()


//-------------------------------------------------------------------------------
// @ IvBoundingSphere::Set()
//-------------------------------------------------------------------------------
// Set bounding sphere based on set of points
//-------------------------------------------------------------------------------
void
IvBoundingSphere::Set( const IvVector3* points, unsigned int numPoints )
{
    ASSERT( points );
    // compute minimal and maximal bounds
    IvVector3 min(points[0]), max(points[0]);
    unsigned int i;
    for ( i = 1; i < numPoints; ++i )
    {
        if (points[i].x < min.x)
            min.x = points[i].x;
        else if (points[i].x > max.x )
            max.x = points[i].x;
        if (points[i].y < min.y)
            min.y = points[i].y;
        else if (points[i].y > max.y )
            max.y = points[i].y;
        if (points[i].z < min.z)
            min.z = points[i].z;
        else if (points[i].z > max.z )
            max.z = points[i].z;
    }
    // compute center and radius
    mCenter = 0.5f*(min + max);
    float maxDistance = ::DistanceSquared( mCenter, points[0] );
    for ( i = 1; i < numPoints; ++i )
    {
        float dist = ::DistanceSquared( mCenter, points[i] );
        if (dist > maxDistance)
            maxDistance = dist;
    }
    mRadius = ::IvSqrt( maxDistance );
}


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Transform()
// ---------------------------------------------------------------------------
// Transforms sphere into new space
//-----------------------------------------------------------------------------
IvBoundingSphere    
IvBoundingSphere::Transform( float scale, const IvQuat& rotate, 
    const IvVector3& translate ) const
{
    return IvBoundingSphere( rotate.Rotate(mCenter) + translate, mRadius*scale );

}   // End of IvBoundingSphere::Transform()


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Transform()
// ---------------------------------------------------------------------------
// Transforms sphere into new space
//-----------------------------------------------------------------------------
IvBoundingSphere    
IvBoundingSphere::Transform( float scale, const IvMatrix33& rotate, 
    const IvVector3& translate ) const
{
    return IvBoundingSphere( rotate*mCenter + translate, mRadius*scale );

}   // End of IvBoundingSphere::Transform()


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and sphere
//-----------------------------------------------------------------------------
bool 
IvBoundingSphere::Intersect( const IvBoundingSphere& other ) const
{
    // do sphere check
    float radiusSum = mRadius + other.mRadius;
    IvVector3 centerDiff = other.mCenter - mCenter; 
    float distancesq = centerDiff.LengthSquared();

    // if distance squared < sum of radii squared, collision!
    return ( distancesq <= radiusSum*radiusSum );
}


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and line
//-----------------------------------------------------------------------------
bool
IvBoundingSphere::Intersect( const IvLine3& line ) const
{
    // compute intermediate values
    IvVector3 w = mCenter - line.GetOrigin();
    float wsq = w.Dot(w);
    float proj = w.Dot(line.GetDirection());
    float rsq = mRadius*mRadius;
    float vsq = line.GetDirection().Dot(line.GetDirection());

    // test length of difference vs. radius
    return ( vsq*wsq - proj*proj <= vsq*rsq );
}


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and ray
//-----------------------------------------------------------------------------
bool
IvBoundingSphere::Intersect( const IvRay3& ray ) const
{
    // compute intermediate values
    IvVector3 w = mCenter - ray.GetOrigin();
    float wsq = w.Dot(w);
    float proj = w.Dot(ray.GetDirection());
    float rsq = mRadius*mRadius;

    // if sphere behind ray, no intersection
    if ( proj < 0.0f && wsq > rsq )
        return false;
    float vsq = ray.GetDirection().Dot(ray.GetDirection());

    // test length of difference vs. radius
    return ( vsq*wsq - proj*proj <= vsq*rsq );
}


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and line segment
//-----------------------------------------------------------------------------
bool
IvBoundingSphere::Intersect( const IvLineSegment3& segment ) const
{
    // compute intermediate values
    IvVector3 w = mCenter - segment.GetOrigin();
    float wsq = w.Dot(w);
    float proj = w.Dot(segment.GetDirection());
    float rsq = mRadius*mRadius;

    // if sphere outside segment, no intersection
    if ( (proj < 0.0f || proj > 1.0f) && wsq > rsq )
        return false;
    float vsq = segment.GetDirection().Dot(segment.GetDirection());

    // test length of difference vs. radius
    return ( vsq*wsq - proj*proj <= vsq*rsq );
}


//----------------------------------------------------------------------------
// @ IvBoundingSphere::Classify()
// ---------------------------------------------------------------------------
// Compute signed distance between sphere and plane
//-----------------------------------------------------------------------------
float
IvBoundingSphere::Classify( const IvPlane& plane ) const
{
    float distance = plane.Test( mCenter );
    if ( distance > mRadius )
    {
        return distance-mRadius;
    }
    else if ( distance < -mRadius )
    {
        return distance+mRadius;
    }
    else
    {
        return 0.0f;
    }

}   // End of IvBoundingSphere::Classify()



//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two spheres together to create a new one
//-----------------------------------------------------------------------------
void
Merge( IvBoundingSphere& result, 
       const IvBoundingSphere& s0, const IvBoundingSphere& s1 )
{
    // get differences between them
    IvVector3 diff = s1.mCenter - s0.mCenter;
    float distsq = diff.Dot(diff);
    float radiusdiff = s1.mRadius - s0.mRadius;

    // if one sphere inside other
    if ( distsq <= radiusdiff*radiusdiff )
    {
        if ( s0.mRadius > s1.mRadius )
            result = s0;
        else
            result = s1;
        return;
    }

    // build new sphere
    float dist = ::IvSqrt( distsq );
    float radius = 0.5f*( s0.mRadius + s1.mRadius + dist );
    IvVector3 center = s0.mCenter;
    if (!IvIsZero( dist ))
        center += ((radius-s0.mRadius)/dist)*diff;

    result.SetRadius( radius );
    result.SetCenter( center );

}   // End of ::Merge()


//----------------------------------------------------------------------------
// @ IvBoundingSphere::ComputeCollision()
// ---------------------------------------------------------------------------
// Compute parameters for collision between sphere and sphere
//-----------------------------------------------------------------------------
bool 
IvBoundingSphere::ComputeCollision( const IvBoundingSphere& other, IvVector3& collisionNormal, 
                      IvVector3& collisionPoint, float& penetration ) const
{
    // do sphere check
    float radiusSum = mRadius + other.mRadius;
    collisionNormal = other.mCenter - mCenter;  
    float distancesq = collisionNormal.LengthSquared();
    // if distance squared < sum of radii squared, collision!
    if ( distancesq <= radiusSum*radiusSum )
    {
        // handle collision

        // penetration is distance - radii
        float distance = ::IvSqrt(distancesq);
        penetration = radiusSum - distance;
        collisionNormal.Normalize();

        // collision point is average of penetration
        collisionPoint = 0.5f*(mCenter + mRadius*collisionNormal) 
                        + 0.5f*(other.mCenter - other.mRadius*collisionNormal);

        return true;
    }

    return false;

}  // End of ::ComputeCollision()
