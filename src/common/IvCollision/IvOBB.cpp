//===============================================================================
// @ IvOBB.cpp
// 
// Oriented bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvAssert.h>
#include "IvCovariance.h"
#include <IvLine3.h>
#include <IvLineSegment3.h>
#include <IvMath.h>
#include <IvMatrix33.h>
#include "IvOBB.h"
#include <IvPlane.h>
#include <IvQuat.h>
#include <IvRay3.h>
#include <IvVector3.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvOBB::IvOBB()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvOBB::IvOBB(const IvOBB& other) : 
    mCenter( other.mCenter ),
    mRotation( other.mRotation ),
    mExtents( other.mExtents )
{

}   // End of IvOBB::IvOBB()


//-------------------------------------------------------------------------------
// @ IvOBB::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvOBB&
IvOBB::operator=(const IvOBB& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mCenter = other.mCenter;
    mRotation = other.mRotation;
    mExtents = other.mExtents;
    
    return *this;

}   // End of IvOBB::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvOBB& source)
{
    out << source.mCenter;
    out << ' ' << source.mExtents;
    out << ' ' << source.mRotation;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvOBB::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvOBB::operator==( const IvOBB& other ) const
{
    if ( other.mCenter == mCenter && other.mRotation == mRotation
         && other.mExtents == mExtents )
        return true;

    return false;   
}   // End of IvOBB::operator==()


//-------------------------------------------------------------------------------
// @ IvOBB::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvOBB::operator!=( const IvOBB& other ) const
{
    if ( other.mCenter != mCenter || other.mRotation != mRotation
         || other.mExtents != mExtents )
        return true;

    return false;

}   // End of IvOBB::operator!=()


//-------------------------------------------------------------------------------
// @ IvOBB::Set()
//-------------------------------------------------------------------------------
// Set OBB based on set of points
//-------------------------------------------------------------------------------
void
IvOBB::Set( const IvVector3* points, unsigned int nPoints )
{
    ASSERT( points );

    IvVector3 centroid;

    // compute covariance matrix
    IvMatrix33 C;
    IvComputeCovarianceMatrix( C, centroid, points, nPoints );

    // get basis vectors
    IvVector3 basis[3];
    IvGetRealSymmetricEigenvectors( basis[0], basis[1], basis[2], C );
    mRotation.SetColumns( basis[0], basis[1], basis[2] );

    IvVector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    IvVector3 max(FLT_MIN, FLT_MIN, FLT_MIN);

    // compute min, max projections of box on axes
    // for each point do 
    unsigned int i;
    for ( i = 0; i < nPoints; ++i )
    {
        IvVector3 diff = points[i]-centroid;
        for (int j = 0; j < 3; ++j)
        {
            float length = diff.Dot(basis[j]);
            if (length > max[j])
            {
                max[j] = length;
            }
            else if (length < min[j])
            {
                min[j] = length;
            }
        }
    }

    // compute center, extents
    mCenter = centroid;
    for ( i = 0; i < 3; ++i )
    {
        mCenter += 0.5f*(min[i]+max[i])*basis[i];
        mExtents[i] = 0.5f*(max[i]-min[i]);
    }

}   // End of IvOBB::Set()


//----------------------------------------------------------------------------
// @ IvOBB::Transform()
// ---------------------------------------------------------------------------
// Transforms OBB into new space
//-----------------------------------------------------------------------------
IvOBB    
IvOBB::Transform( float scale, const IvQuat& rotate, 
    const IvVector3& translate ) const
{
    IvOBB result;
    IvMatrix33 rotationMatrix(rotate);

    result.SetExtents( scale*mExtents );
    result.SetCenter( rotationMatrix*mCenter + translate );
    result.SetRotation( rotationMatrix*mRotation );

    return result;

}   // End of IvOBB::Transform()


//----------------------------------------------------------------------------
// @ IvOBB::Transform()
// ---------------------------------------------------------------------------
// Transforms OBB into new space
//-----------------------------------------------------------------------------
IvOBB    
IvOBB::Transform( float scale, const IvMatrix33& rotate, 
    const IvVector3& translate ) const
{
    IvOBB result;

    result.SetExtents( scale*mExtents );
    result.SetCenter( rotate*mCenter + translate );
    result.SetRotation( rotate*mRotation );

    return result;
    
}   // End of IvOBB::Transform()


//----------------------------------------------------------------------------
// @ IvOBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and OBB
//
// The efficiency of this could be improved slightly by computing factors only
// as we need them.  For example, we could compute only the first row of R
// before the first axis test, then the second row, etc.  It has been left this
// way because it's clearer.
//-----------------------------------------------------------------------------
bool 
IvOBB::Intersect( const IvOBB& other ) const
{
    // extent vectors
    const IvVector3& a = mExtents;    
    const IvVector3& b = other.mExtents;

    // test factors
    float cTest, aTest, bTest;
    bool parallelAxes = false;

    // transpose of rotation of B relative to A, i.e. (R_b^T * R_a)^T
    IvMatrix33 Rt = ::Transpose(mRotation)*other.mRotation;

    // absolute value of relative rotation matrix
    IvMatrix33 Rabs;  
    for ( unsigned int i = 0; i < 3; ++i )
    {
        for ( unsigned int j = 0; j < 3; ++j )
        {
            Rabs(i,j) = IvAbs( Rt(i,j ) );
            // if magnitude of dot product between axes is close to one
            if ( Rabs(i,j) + kEpsilon >= 1.0f )
            {
                // then box A and box B have near-parallel axes
                parallelAxes = true;
            }
        }
    }
    
    // relative translation (in A's frame)
    IvVector3 c = (other.mCenter - mCenter)*mRotation;

    // separating axis A0
    cTest = ::IvAbs(c.x);
    aTest = a.x;
    bTest = b.x*Rabs(0,0)+b.y*Rabs(0,1)+b.z*Rabs(0,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1
    cTest = ::IvAbs(c.y);
    aTest = a.y;
    bTest = b.x*Rabs(1,0)+b.y*Rabs(1,1)+b.z*Rabs(1,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2
    cTest = ::IvAbs(c.z);
    aTest = a.z;
    bTest = b.x*Rabs(2,0)+b.y*Rabs(2,1)+b.z*Rabs(2,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B0
    cTest = ::IvAbs( c.x*Rt(0,0) + c.y*Rt(1,0) + c.z*Rt(2,0) );
    aTest = a.x*Rabs(0,0)+a.y*Rabs(1,0)+a.z*Rabs(2,0);
    bTest = b.x;
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B1
    cTest = ::IvAbs( c.x*Rt(0,1) + c.y*Rt(1,1) + c.z*Rt(2,1) );
    aTest = a.x*Rabs(0,1)+a.y*Rabs(1,1)+a.z*Rabs(2,1);
    bTest = b.y;
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B2
    cTest = ::IvAbs( c.x*Rt(0,2) + c.y*Rt(1,2) + c.z*Rt(2,2) );
    aTest = a.x*Rabs(0,2)+a.y*Rabs(1,2)+a.z*Rabs(2,2);
    bTest = b.z;
    if ( cTest > aTest + bTest )
        return false;

    // if the two boxes have parallel axes, we're done, intersection
    if ( parallelAxes )
        return true;

    // separating axis A0 x B0
    cTest = ::IvAbs(c.z*Rt(1,0)-c.y*Rt(2,0));
    aTest = a.y*Rabs(2,0) + a.z*Rabs(1,0);
    bTest = b.y*Rabs(0,2) + b.z*Rabs(0,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A0 x B1
    cTest = ::IvAbs(c.z*Rt(1,1)-c.y*Rt(2,1));
    aTest = a.y*Rabs(2,1) + a.z*Rabs(1,1);
    bTest = b.x*Rabs(0,2) + b.z*Rabs(0,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A0 x B2
    cTest = ::IvAbs(c.z*Rt(1,2)-c.y*Rt(2,2));
    aTest = a.y*Rabs(2,2) + a.z*Rabs(1,2);
    bTest = b.x*Rabs(0,1) + b.y*Rabs(0,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B0
    cTest = ::IvAbs(c.x*Rt(2,0)-c.z*Rt(0,0));
    aTest = a.x*Rabs(2,0) + a.z*Rabs(0,0);
    bTest = b.y*Rabs(1,2) + b.z*Rabs(1,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B1
    cTest = ::IvAbs(c.x*Rt(2,1)-c.z*Rt(0,1));
    aTest = a.x*Rabs(2,1) + a.z*Rabs(0,1);
    bTest = b.x*Rabs(1,2) + b.z*Rabs(1,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B2
    cTest = ::IvAbs(c.x*Rt(2,2)-c.z*Rt(0,2));
    aTest = a.x*Rabs(2,2) + a.z*Rabs(0,2);
    bTest = b.x*Rabs(1,1) + b.y*Rabs(1,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B0
    cTest = ::IvAbs(c.y*Rt(0,0)-c.x*Rt(1,0));
    aTest = a.x*Rabs(1,0) + a.y*Rabs(0,0);
    bTest = b.y*Rabs(2,2) + b.z*Rabs(2,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B1
    cTest = ::IvAbs(c.y*Rt(0,1)-c.x*Rt(1,1));
    aTest = a.x*Rabs(1,1) + a.y*Rabs(0,1);
    bTest = b.x*Rabs(2,2) + b.z*Rabs(2,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B2
    cTest = ::IvAbs(c.y*Rt(0,2)-c.x*Rt(1,2));
    aTest = a.x*Rabs(1,2) + a.y*Rabs(0,2);
    bTest = b.x*Rabs(2,1) + b.y*Rabs(2,0);
    if ( cTest > aTest + bTest )
        return false;

    // all tests failed, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ IvOBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and line
//-----------------------------------------------------------------------------
bool
IvOBB::Intersect( const IvLine3& line ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // compute difference vector
    IvVector3 diff = mCenter - line.GetOrigin();

    // for each axis do
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        IvVector3 axis = mRotation.GetColumn( i );

        // project relative vector onto axis
        float e = axis.Dot( diff );
        float f = line.GetDirection().Dot( axis );

        // ray is parallel to plane
        if ( IvIsZero( f ) )
        {
            // ray passes by box
            if ( -e - mExtents[i] > 0.0f || -e + mExtents[i] > 0.0f )
                return false;
            continue;
        }

        float s = (e - mExtents[i])/f;
        float t = (e + mExtents[i])/f;

        // fix order
        if ( s > t )
        {
            float temp = s;
            s = t;
            t = temp;
        }

        // adjust min and max values
        if ( s > maxS )
            maxS = s;
        if ( t < minT )
            minT = t;

        // check for intersection failure
        if ( maxS > minT )
            return false;
    }

    // done, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ IvOBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and ray
//-----------------------------------------------------------------------------
bool
IvOBB::Intersect( const IvRay3& ray ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // compute difference vector
    IvVector3 diff = mCenter - ray.GetOrigin();

    // for each axis do
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        IvVector3 axis = mRotation.GetColumn( i );

        // project relative vector onto axis
        float e = axis.Dot( diff );
        float f = ray.GetDirection().Dot( axis );

        // ray is parallel to plane
        if ( IvIsZero( f ) )
        {
            // ray passes by box
            if ( -e - mExtents[i] > 0.0f || -e + mExtents[i] > 0.0f )
                return false;
            continue;
        }

        float s = (e - mExtents[i])/f;
        float t = (e + mExtents[i])/f;

        // fix order
        if ( s > t )
        {
            float temp = s;
            s = t;
            t = temp;
        }

        // adjust min and max values
        if ( s > maxS )
            maxS = s;
        if ( t < minT )
            minT = t;

        // check for intersection failure
        if ( minT < 0.0f || maxS > minT )
            return false;
    }

    // done, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ IvOBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and line segment
//-----------------------------------------------------------------------------
bool
IvOBB::Intersect( const IvLineSegment3& segment ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // compute difference vector
    IvVector3 diff = mCenter - segment.GetOrigin();

    // for each axis do
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        IvVector3 axis = mRotation.GetColumn( i );

        // project relative vector onto axis
        float e = axis.Dot( diff );
        float f = segment.GetDirection().Dot( axis );

        // ray is parallel to plane
        if ( IvIsZero( f ) )
        {
            // ray passes by box
            if ( -e - mExtents[i] > 0.0f || -e + mExtents[i] > 0.0f )
                return false;
            continue;
        }

        float s = (e - mExtents[i])/f;
        float t = (e + mExtents[i])/f;

        // fix order
        if ( s > t )
        {
            float temp = s;
            s = t;
            t = temp;
        }

        // adjust min and max values
        if ( s > maxS )
            maxS = s;
        if ( t < minT )
            minT = t;

        // check for intersection failure
        if ( minT < 0.0f || maxS > 1.0f || maxS > minT )
            return false;
    }

    // done, have intersection
    return true;

}   // End of IvOBB::Intersect()


//----------------------------------------------------------------------------
// @ IvOBB::Classify()
// ---------------------------------------------------------------------------
// Return signed distance between OBB and plane
//-----------------------------------------------------------------------------
float IvOBB::Classify( const IvPlane& plane ) const
{
    IvVector3 xNormal = plane.GetNormal()*mRotation;
    // maximum extent in direction of plane normal 
    float r = ::IvAbs(mExtents.x*xNormal.x) 
            + ::IvAbs(mExtents.y*xNormal.y) 
            + ::IvAbs(mExtents.z*xNormal.z);
    // signed distance between box center and plane
    float d = plane.Test(mCenter);

    // return signed distance
    if (::IvAbs(d) < r)
        return 0.0f;
    else if (d < 0.0f)
        return d + r;
    else
        return d - r;

}   // End of IvOBB::Classify()


//----------------------------------------------------------------------------
// @ Merge()
// ---------------------------------------------------------------------------
// Merge two OBBs together to create a new one
//-----------------------------------------------------------------------------
void
Merge( IvOBB& result, const IvOBB& b0, const IvOBB& b1 )
{
    // new center is average of original centers
    IvVector3 newCenter = (b0.mCenter + b1.mCenter)*0.5f;

    // new axes are sum of rotations (as quaternions)
    IvQuat q0( b0.mRotation );
    IvQuat q1( b1.mRotation );
    IvQuat q;
    // this shouldn't happen, but just in case
    if ( q0.Dot(q1) < 0.0f )
        q = q0-q1;
    else
        q = q0+q1;
    q.Normalize();
    IvMatrix33 newRotation( q );

    // new extents are projections of old extents on new axes
    // for each axis do
    IvVector3 newExtents;
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        IvVector3 axis = newRotation.GetColumn( i );

        // get difference between this box center and other box center
        IvVector3 centerDiff = b0.mCenter - newCenter;

        // rotate into box 0's space
        IvVector3 xAxis = axis*b0.mRotation;
        // maximum extent in direction of axis
        newExtents[i] = ::IvAbs(centerDiff.x*axis.x) 
                      + ::IvAbs(centerDiff.y*axis.y)  
                      + ::IvAbs(centerDiff.z*axis.z) 
                      + ::IvAbs(xAxis.x*b0.mExtents.x) 
                      + ::IvAbs(xAxis.y*b0.mExtents.y) 
                      + ::IvAbs(xAxis.z*b0.mExtents.z);

        // get difference between this box center and other box center
        centerDiff = b1.mCenter - newCenter;

        // rotate into box 1's space
        xAxis = axis*b1.mRotation;
        // maximum extent in direction of axis
        float maxExtent = ::IvAbs(centerDiff.x*axis.x) 
                      + ::IvAbs(centerDiff.y*axis.y)  
                      + ::IvAbs(centerDiff.z*axis.z) 
                      + ::IvAbs(xAxis.x*b1.mExtents.x) 
                      + ::IvAbs(xAxis.y*b1.mExtents.y) 
                      + ::IvAbs(xAxis.z*b1.mExtents.z);
        // if greater than box0's result, use it
        if (maxExtent > newExtents[i])
            newExtents[i] = maxExtent;
    }

    // copy at end, in case user is passing in b0 or b1 as result
    result.mCenter = newCenter;
    result.mRotation = newRotation;
    result.mExtents = newExtents;

}   // End of Merge()
