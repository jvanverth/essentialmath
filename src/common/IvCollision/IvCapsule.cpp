//===============================================================================
// @ IvCapsule.cpp
// 
// Capsule collision class
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

#include <IvAssert.h>
#include "IvCapsule.h"
#include "IvCovariance.h"
#include <IvLine3.h>
#include <IvMath.h>
#include <IvMatrix33.h>
#include <IvPlane.h>
#include <IvVector3.h>
#include <float.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvCapsule::IvCapsule()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvCapsule::IvCapsule(const IvCapsule& other) : 
    mSegment( other.mSegment ),
    mRadius( other.mRadius )
{

}   // End of IvCapsule::IvCapsule()


//-------------------------------------------------------------------------------
// @ IvCapsule::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvCapsule&
IvCapsule::operator=(const IvCapsule& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mSegment = other.mSegment;
    mRadius = other.mRadius;
    
    return *this;

}   // End of IvCapsule::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvCapsule& source)
{
    out << source.mSegment;
    out << ' ' << source.mRadius;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvCapsule::Length()
//-------------------------------------------------------------------------------
// Vector length
//-------------------------------------------------------------------------------
float 
IvCapsule::Length() const
{
    return mSegment.Length() + 2.0f*mRadius;

}   // End of IvCapsule::Length()


//-------------------------------------------------------------------------------
// @ IvCapsule::LengthSquared()
//-------------------------------------------------------------------------------
// Capsule length squared (avoids square root)
//-------------------------------------------------------------------------------
float 
IvCapsule::LengthSquared() const
{
    return mSegment.LengthSquared();

}   // End of IvCapsule::LengthSquared()


//-------------------------------------------------------------------------------
// @ IvCapsule::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvCapsule::operator==( const IvCapsule& other ) const
{
    if ( other.mSegment == mSegment && other.mRadius == mRadius )
        return true;

    return false;   
}   // End of IvCapsule::operator==()


//-------------------------------------------------------------------------------
// @ IvCapsule::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvCapsule::operator!=( const IvCapsule& other ) const
{
    if ( other.mSegment == mSegment && other.mRadius == mRadius )
        return false;

    return true;
}   // End of IvCapsule::operator!=()


//-------------------------------------------------------------------------------
// @ IvCapsule::Set()
//-------------------------------------------------------------------------------
// Set capsule based on set of points
//-------------------------------------------------------------------------------
void
IvCapsule::Set( const IvVector3* points, unsigned int nPoints )
{
    ASSERT( points );
    IvVector3 centroid;

    // compute covariance matrix
    IvMatrix33 C;
    IvComputeCovarianceMatrix( C, centroid, points, nPoints );

    // get main axis
    IvVector3 w, u, v;
    IvGetRealSymmetricEigenvectors( w, u, v, C );

    float maxDistSq = 0.0f;

    // for each point do
    unsigned int i;
    for ( i = 0; i < nPoints; ++i )
    {
        // compute radius
        IvVector3 diff = points[i]-centroid;
        float proj = diff.Dot(w);

        float distSq = diff.Dot(diff) - proj*proj;
        if (distSq > maxDistSq)
          maxDistSq = distSq;
    }

    mRadius = ::IvSqrt(maxDistSq);

    // now set endcaps
    // for each point do
    float t0 = FLT_MAX;
    float t1 = FLT_MIN;
    for ( i = 0; i < nPoints; ++i )
    {
        IvVector3 localTrans = points[i]-centroid;
        float uCoord = localTrans.Dot(u);
        float vCoord = localTrans.Dot(v);
        float wCoord = localTrans.Dot(w);

        float radical = maxDistSq - uCoord*uCoord - vCoord*vCoord;
        if ( radical > kEpsilon )
            radical = ::IvSqrt( radical );
        else
            radical = 0.0f;

        float test = wCoord + radical;
        if ( test < t0 )
            t0 = test;

        test = wCoord - radical;
        if ( test > t1 )
            t1 = test;
    }

    // set up segment
    if ( t0 < t1 )
    {
        mSegment.Set( centroid + t0*w, centroid + t1*w );
    }
    else
    {
        // is sphere
        IvVector3 center = centroid + 0.5f*(t0+t1)*w;
        mSegment.Set( center, center );
    }

}   // End of IvCapsule::Set()


//----------------------------------------------------------------------------
// @ IvCapsule::Transform()
// ---------------------------------------------------------------------------
// Transforms segment into new space
//-----------------------------------------------------------------------------
IvCapsule   
IvCapsule::Transform( float scale, const IvQuat& rotate, 
    const IvVector3& translate ) const
{
    return IvCapsule( mSegment.Transform( scale, rotate, translate ), mRadius*scale );

}   // End of IvCapsule::Transform()


//----------------------------------------------------------------------------
// @ IvCapsule::Transform()
// ---------------------------------------------------------------------------
// Transforms segment into new space
//-----------------------------------------------------------------------------
IvCapsule   
IvCapsule::Transform( float scale, const IvMatrix33& rotate, 
    const IvVector3& translate ) const
{
    return IvCapsule( mSegment.Transform( scale, rotate, translate ), 
        mRadius*scale );

}   // End of IvCapsule::Transform()


//----------------------------------------------------------------------------
// @ IvCapsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and capsule
//-----------------------------------------------------------------------------
bool 
IvCapsule::Intersect( const IvCapsule& other ) const
{
    float radiusSum = mRadius + other.mRadius;

    // if colliding
    float s, t;
    float distancesq = ::DistanceSquared( mSegment, other.mSegment, s, t ); 

    return ( distancesq <= radiusSum*radiusSum );
}


//----------------------------------------------------------------------------
// @ IvCapsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and line
//-----------------------------------------------------------------------------
bool
IvCapsule::Intersect( const IvLine3& line ) const
{
    // test distance between line and segment vs. radius
    float s_c, t_c;
    return ( ::DistanceSquared( mSegment, line, s_c, t_c ) <= mRadius*mRadius );
}


//----------------------------------------------------------------------------
// @ IvCapsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and ray
//-----------------------------------------------------------------------------
bool
IvCapsule::Intersect( const IvRay3& ray ) const
{
    // test distance between ray and segment vs. radius
    float s_c, t_c;
    return ( ::DistanceSquared( mSegment, ray, s_c, t_c ) <= mRadius*mRadius );

}


//----------------------------------------------------------------------------
// @ IvCapsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and line segment
//-----------------------------------------------------------------------------
bool
IvCapsule::Intersect( const IvLineSegment3& segment ) const
{
    // test distance between segment and segment vs. radius
    float s_c, t_c;
    return ( ::DistanceSquared( mSegment, segment, s_c, t_c ) <= mRadius*mRadius );

}


//----------------------------------------------------------------------------
// @ IvCapsule::Classify()
// ---------------------------------------------------------------------------
// Return signed distance between capsule and plane
//-----------------------------------------------------------------------------
float
IvCapsule::Classify( const IvPlane& plane ) const
{
    float s0 = plane.Test( mSegment.GetEndpoint0() );
    float s1 = plane.Test( mSegment.GetEndpoint1() );

    // points on opposite sides or intersecting plane
    if ( s0*s1 <= 0.0f )
        return 0.0f;

    // intersect if either endpoint is within radius distance of plane
    if( ::IvAbs(s0) <= mRadius || ::IvAbs(s1) <= mRadius )
        return 0.0f;

    // return signed distance
    return ( ::IvAbs(s0) < ::IvAbs(s1) ? s0 : s1 );
}


//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two capsules together to create a new one
// See _3D Game Engine Design_ (Eberly) for more details
//-----------------------------------------------------------------------------
void
Merge( IvCapsule& result, const IvCapsule& b0, const IvCapsule& b1 )
{
    // 1) get line through center

    // origin is average of centers
    IvVector3 origin = 0.5f*(b0.mSegment.GetCenter() + b1.mSegment.GetCenter());
    
    // direction is average of directions
    IvVector3 direction0 = b0.mSegment.GetDirection();
    direction0.Normalize();
    IvVector3 direction1 = b1.mSegment.GetDirection();
    direction1.Normalize();
    if (direction0.Dot(direction1) < 0.0f)
        direction1 = -direction1;
    IvVector3 direction = direction0 + direction1;

    // create line
    IvLine3 line( origin, direction );

    // 2) compute radius

    // radius will be the maximum of the distance to each endpoint, 
    // plus the corresponding capsule radius
    float t;
    float radius = Distance( line, b0.mSegment.GetEndpoint0(), t ) + b0.mRadius;
    float tempRadius = Distance( line, b0.mSegment.GetEndpoint1(), t ) + b0.mRadius;
    if ( tempRadius > radius )
        radius = tempRadius;
    tempRadius = Distance( line, b1.mSegment.GetEndpoint0(), t ) + b1.mRadius;
    if ( tempRadius > radius )
        radius = tempRadius;
    tempRadius = Distance( line, b1.mSegment.GetEndpoint1(), t ) + b1.mRadius;
    if ( tempRadius > radius )
        radius = tempRadius;

    // 3) compute parameters of endpoints on line

    // we compute a sphere at each original endpoint, and set the endcaps of the
    // new capsule around them, minimizing size

    // capsule 0, endpoint 0
    float radiusDiffSq = radius - b0.GetRadius();
    radiusDiffSq *= radiusDiffSq;
    IvVector3 originDiff = line.GetOrigin() - b0.mSegment.GetEndpoint0();
    // compute coefficients for quadratic
    float halfb = line.GetDirection().Dot( originDiff );
    float c = originDiff.Dot(originDiff) - radiusDiffSq;
    // this is sqrt( b^2 - 4ac )/2, but a lot of terms have cancelled out
    float radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = ::IvSqrt( radical );
    else
        radical = 0.0f;
    // and this is adding -b/2 +/- the above
    float t0 = -halfb - radical;
    float t1 = -halfb + radical;

    // capsule 0, endpoint 1
    originDiff = line.GetOrigin() - b0.mSegment.GetEndpoint1();
    halfb = line.GetDirection().Dot( originDiff );
    c = originDiff.Dot(originDiff) - radiusDiffSq;
    radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = ::IvSqrt( radical );
    else
        radical = 0.0f;
    float param = -halfb - radical;
    if ( param < t0 )
        t0 = param;
    param = -halfb + radical;
    if ( param > t1 )
        t1 = param;

    // capsule 1, endpoint 0
    radiusDiffSq = radius - b1.GetRadius();
    radiusDiffSq *= radiusDiffSq;
    originDiff = line.GetOrigin() - b1.mSegment.GetEndpoint0();
    halfb = line.GetDirection().Dot( originDiff );
    c = originDiff.Dot(originDiff) - radiusDiffSq;
    radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = ::IvSqrt( radical );
    else
        radical = 0.0f;
    param = -halfb - radical;
    if ( param < t0 )
        t0 = param;
    param = -halfb + radical;
    if ( param > t1 )
        t1 = param;

     // capsule 1, endpoint 1
    originDiff = line.GetOrigin() - b1.mSegment.GetEndpoint1();
    halfb = line.GetDirection().Dot( originDiff );
    c = originDiff.Dot(originDiff) - radiusDiffSq;
    radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = ::IvSqrt( radical );
    else
        radical = 0.0f;
    param = -halfb - radical;
    if ( param < t0 )
        t0 = param;
    param = -halfb + radical;
    if ( param > t1 )
        t1 = param;

    // set capsule
    result.mRadius = radius;
    if ( t0 < t1 )
    {
        result.mSegment.Set( line.GetOrigin() + t0*line.GetDirection(), 
                      line.GetOrigin() + t1*line.GetDirection() );
    }
    else
    {
        // is sphere
        IvVector3 center = line.GetOrigin() + 0.5f*(t0+t1)*line.GetDirection();
        result.mSegment.Set( center, center );
    }

}   // End of ::Merge()



//----------------------------------------------------------------------------
// @ IvCapsule::ComputeCollision()
// ---------------------------------------------------------------------------
// Compute parameters for collision between capsule and capsule
//-----------------------------------------------------------------------------
bool 
IvCapsule::ComputeCollision( const IvCapsule& other, IvVector3& collisionNormal, 
                      IvVector3& collisionPoint, float& penetration ) const
{
    float radiusSum = mRadius + other.mRadius;

    // if colliding
    float s, t;
    float distancesq = DistanceSquared( mSegment, other.mSegment, s, t );

    if ( distancesq <= radiusSum*radiusSum )
    {
        // calculate our values
        IvVector3 nearPoint0 = (1.0f-s)*mSegment.GetEndpoint0() + s*mSegment.GetEndpoint1();
        IvVector3 nearPoint1 = (1.0f-t)*other.mSegment.GetEndpoint0() + t*other.mSegment.GetEndpoint1();

        collisionNormal = nearPoint1 - nearPoint0;  

        // penetration is distance - radii
        float distance = ::IvSqrt(distancesq);
        penetration = radiusSum - distance;
        collisionNormal.Normalize();

        // collision point is average of penetration
        // weighted towards lighter object
        float t = 0.5f;//mMass/(mMass + other->mMass);
        collisionPoint = (1.0f-t)*(nearPoint0 + mRadius*collisionNormal)
                        + t*(nearPoint1 - other.mRadius*collisionNormal);

        return true;
    }

    return false;
}


