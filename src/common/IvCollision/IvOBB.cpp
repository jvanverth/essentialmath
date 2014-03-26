//===============================================================================
// @ IvOBB.cpp
// 
// Oriented bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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
    cTest = ::IvAbs(c.GetX());
    aTest = a.GetX();
    bTest = b.GetX()*Rabs(0,0)+b.GetY()*Rabs(0,1)+b.GetZ()*Rabs(0,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1
    cTest = ::IvAbs(c.GetY());
    aTest = a.GetY();
    bTest = b.GetX()*Rabs(1,0)+b.GetY()*Rabs(1,1)+b.GetZ()*Rabs(1,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2
    cTest = ::IvAbs(c.GetZ());
    aTest = a.GetZ();
    bTest = b.GetX()*Rabs(2,0)+b.GetY()*Rabs(2,1)+b.GetZ()*Rabs(2,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B0
    cTest = ::IvAbs( c.GetX()*Rt(0,0) + c.GetY()*Rt(1,0) + c.GetZ()*Rt(2,0) );
    aTest = a.GetX()*Rabs(0,0)+a.GetY()*Rabs(1,0)+a.GetZ()*Rabs(2,0);
    bTest = b.GetX();
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B1
    cTest = ::IvAbs( c.GetX()*Rt(0,1) + c.GetY()*Rt(1,1) + c.GetZ()*Rt(2,1) );
    aTest = a.GetX()*Rabs(0,1)+a.GetY()*Rabs(1,1)+a.GetZ()*Rabs(2,1);
    bTest = b.GetY();
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B2
    cTest = ::IvAbs( c.GetX()*Rt(0,2) + c.GetY()*Rt(1,2) + c.GetZ()*Rt(2,2) );
    aTest = a.GetX()*Rabs(0,2)+a.GetY()*Rabs(1,2)+a.GetZ()*Rabs(2,2);
    bTest = b.GetZ();
    if ( cTest > aTest + bTest )
        return false;

    // if the two boxes have parallel axes, we're done, intersection
    if ( parallelAxes )
        return true;

    // separating axis A0 x B0
    cTest = ::IvAbs(c.GetZ()*Rt(1,0)-c.GetY()*Rt(2,0));
    aTest = a.GetY()*Rabs(2,0) + a.GetZ()*Rabs(1,0);
    bTest = b.GetY()*Rabs(0,2) + b.GetZ()*Rabs(0,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A0 x B1
    cTest = ::IvAbs(c.GetZ()*Rt(1,1)-c.GetY()*Rt(2,1));
    aTest = a.GetY()*Rabs(2,1) + a.GetZ()*Rabs(1,1);
    bTest = b.GetX()*Rabs(0,2) + b.GetZ()*Rabs(0,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A0 x B2
    cTest = ::IvAbs(c.GetZ()*Rt(1,2)-c.GetY()*Rt(2,2));
    aTest = a.GetY()*Rabs(2,2) + a.GetZ()*Rabs(1,2);
    bTest = b.GetX()*Rabs(0,1) + b.GetY()*Rabs(0,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B0
    cTest = ::IvAbs(c.GetX()*Rt(2,0)-c.GetZ()*Rt(0,0));
    aTest = a.GetX()*Rabs(2,0) + a.GetZ()*Rabs(0,0);
    bTest = b.GetY()*Rabs(1,2) + b.GetZ()*Rabs(1,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B1
    cTest = ::IvAbs(c.GetX()*Rt(2,1)-c.GetZ()*Rt(0,1));
    aTest = a.GetX()*Rabs(2,1) + a.GetZ()*Rabs(0,1);
    bTest = b.GetX()*Rabs(1,2) + b.GetZ()*Rabs(1,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B2
    cTest = ::IvAbs(c.GetX()*Rt(2,2)-c.GetZ()*Rt(0,2));
    aTest = a.GetX()*Rabs(2,2) + a.GetZ()*Rabs(0,2);
    bTest = b.GetX()*Rabs(1,1) + b.GetY()*Rabs(1,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B0
    cTest = ::IvAbs(c.GetY()*Rt(0,0)-c.GetX()*Rt(1,0));
    aTest = a.GetX()*Rabs(1,0) + a.GetY()*Rabs(0,0);
    bTest = b.GetY()*Rabs(2,2) + b.GetZ()*Rabs(2,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B1
    cTest = ::IvAbs(c.GetY()*Rt(0,1)-c.GetX()*Rt(1,1));
    aTest = a.GetX()*Rabs(1,1) + a.GetY()*Rabs(0,1);
    bTest = b.GetX()*Rabs(2,2) + b.GetZ()*Rabs(2,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B2
    cTest = ::IvAbs(c.GetY()*Rt(0,2)-c.GetX()*Rt(1,2));
    aTest = a.GetX()*Rabs(1,2) + a.GetY()*Rabs(0,2);
    bTest = b.GetX()*Rabs(2,1) + b.GetY()*Rabs(2,0);
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
        if ( ::IsZero( f ) )
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
        if ( ::IsZero( f ) )
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
        if ( ::IsZero( f ) )
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
    float r = ::IvAbs(mExtents.GetX()*xNormal.GetX()) 
            + ::IvAbs(mExtents.GetY()*xNormal.GetY()) 
            + ::IvAbs(mExtents.GetZ()*xNormal.GetZ());
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
// @ ::Merge()
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
        newExtents[i] = ::IvAbs(centerDiff.GetX()*axis.GetX()) 
                      + ::IvAbs(centerDiff.GetY()*axis.GetY())  
                      + ::IvAbs(centerDiff.GetZ()*axis.GetZ()) 
                      + ::IvAbs(xAxis.GetX()*b0.mExtents.GetX()) 
                      + ::IvAbs(xAxis.GetY()*b0.mExtents.GetY()) 
                      + ::IvAbs(xAxis.GetZ()*b0.mExtents.GetZ());

        // get difference between this box center and other box center
        centerDiff = b1.mCenter - newCenter;

        // rotate into box 1's space
        xAxis = axis*b1.mRotation;
        // maximum extent in direction of axis
        float maxExtent = ::IvAbs(centerDiff.GetX()*axis.GetX()) 
                      + ::IvAbs(centerDiff.GetY()*axis.GetY())  
                      + ::IvAbs(centerDiff.GetZ()*axis.GetZ()) 
                      + ::IvAbs(xAxis.GetX()*b1.mExtents.GetX()) 
                      + ::IvAbs(xAxis.GetY()*b1.mExtents.GetY()) 
                      + ::IvAbs(xAxis.GetZ()*b1.mExtents.GetZ());
        // if greater than box0's result, use it
        if (maxExtent > newExtents[i])
            newExtents[i] = maxExtent;
    }

    // copy at end, in case user is passing in b0 or b1 as result
    result.mCenter = newCenter;
    result.mRotation = newRotation;
    result.mExtents = newExtents;

}   // End of ::Merge()
