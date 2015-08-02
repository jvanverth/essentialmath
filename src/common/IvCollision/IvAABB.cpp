//===============================================================================
// @ IvAABB.cpp
// 
// Axis-aligned bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvAABB.h"
#include "IvVector3.h"
#include "IvLine3.h"
#include "IvRay3.h"
#include "IvLineSegment3.h"
#include "IvPlane.h"
#include "IvMath.h"
#include "IvAssert.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvAABB::IvAABB()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
IvAABB::IvAABB(const IvAABB& other) : 
    mMinima( other.mMinima ),
    mMaxima( other.mMaxima )
{

}   // End of IvAABB::IvAABB()


//-------------------------------------------------------------------------------
// @ IvAABB::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
IvAABB&
IvAABB::operator=(const IvAABB& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mMinima = other.mMinima;
    mMaxima = other.mMaxima;
    
    return *this;

}   // End of IvAABB::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvAABB& source)
{
    out << source.mMinima;
    out << ' ' << source.mMaxima;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvAABB::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvAABB::operator==( const IvAABB& other ) const
{
    if ( other.mMinima == mMinima && other.mMaxima == mMaxima )
        return true;

    return false;   
}   // End of IvAABB::operator==()


//-------------------------------------------------------------------------------
// @ IvAABB::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
IvAABB::operator!=( const IvAABB& other ) const
{
    if ( other.mMinima != mMinima || other.mMaxima != mMaxima )
        return true;

    return false;

}   // End of IvAABB::operator!=()


//-------------------------------------------------------------------------------
// @ IvAABB::Set()
//-------------------------------------------------------------------------------
// Set AABB based on set of points
//-------------------------------------------------------------------------------
void
IvAABB::Set( const IvVector3* points, unsigned int numPoints )
{
    ASSERT( points );

    // compute minimal and maximal bounds
    mMinima = points[0];
    mMaxima = points[0];
    for ( unsigned int i = 1; i < numPoints; ++i )
    {
        if (points[i].x < mMinima.x)
            mMinima.x = points[i].x;
        else if (points[i].x > mMaxima.x )
            mMaxima.x = points[i].x;
        if (points[i].y < mMinima.y)
            mMinima.y = points[i].y;
        else if (points[i].y > mMaxima.y )
            mMaxima.y = points[i].y;
        if (points[i].z < mMinima.z)
            mMinima.z = points[i].z;
        else if (points[i].z > mMaxima.z )
            mMaxima.z = points[i].z;
    }
}   // End of IvAABB::Set()


//-------------------------------------------------------------------------------
// @ IvAABB::AddPoint()
//-------------------------------------------------------------------------------
// Add a point to the AABB
//-------------------------------------------------------------------------------
void
IvAABB::AddPoint( const IvVector3& point )
{
    if (point.x < mMinima.x)
        mMinima.x = point.x;
    else if (point.x > mMaxima.x )
        mMaxima.x = point.x;
    if (point.y < mMinima.y)
        mMinima.y = point.y;
    else if (point.y > mMaxima.y )
        mMaxima.y = point.y;
    if (point.z < mMinima.z)
        mMinima.z = point.z;
    else if (point.z > mMaxima.z )
        mMaxima.z = point.z;

}   // End of IvAABB::AddPoint()


//----------------------------------------------------------------------------
// @ IvAABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and AABB
//-----------------------------------------------------------------------------
bool 
IvAABB::Intersect( const IvAABB& other ) const
{
    // if separated in x direction
    if (mMinima.x > other.mMaxima.x || other.mMinima.x > mMaxima.x )
        return false;

    // if separated in y direction
    if (mMinima.y > other.mMaxima.y || other.mMinima.y > mMaxima.y )
        return false;

    // if separated in z direction
    if (mMinima.z > other.mMaxima.z || other.mMinima.z > mMaxima.z )
        return false;

    // no separation, must be intersecting
    return true;
}


//----------------------------------------------------------------------------
// @ IvAABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and line
//-----------------------------------------------------------------------------
bool
IvAABB::Intersect( const IvLine3& line ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // compute intersection parameters and sort
        float s;
        float t;
        float recip = 1.0f/line.GetDirection()[i];
        if ( recip >= 0.0f)
        {
            s = (mMinima[i] - line.GetOrigin()[i])*recip;
            t = (mMaxima[i] - line.GetOrigin()[i])*recip;
        }
        else
        {
            s = (mMaxima[i] - line.GetOrigin()[i])*recip;
            t = (mMinima[i] - line.GetOrigin()[i])*recip;
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
// @ IvAABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and ray
//-----------------------------------------------------------------------------
bool
IvAABB::Intersect( const IvRay3& ray ) const
{
    float maxS = 0.0f;
    float minT = FLT_MAX;
    
    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // compute intersection parameters and sort
        float s;
        float t;
        float recip = 1.0f/ray.GetDirection()[i];
        if ( recip >= 0.0f)
        {
            s = (mMinima[i] - ray.GetOrigin()[i])*recip;
            t = (mMaxima[i] - ray.GetOrigin()[i])*recip;
        }
        else
        {
            s = (mMaxima[i] - ray.GetOrigin()[i])*recip;
            t = (mMinima[i] - ray.GetOrigin()[i])*recip;
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
// @ IvAABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and line segment
//-----------------------------------------------------------------------------
bool
IvAABB::Intersect( const IvLineSegment3& segment ) const
{
    float maxS = 0.0f;
    float minT = 1.0f;
    
    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // compute intersection parameters and sort
        float s;
        float t;
        float recip = 1.0f/segment.GetDirection()[i];
        if ( recip >= 0.0f)
        {
            s = (mMinima[i] - segment.GetOrigin()[i])*recip;
            t = (mMaxima[i] - segment.GetOrigin()[i])*recip;
        }
        else
        {
            s = (mMaxima[i] - segment.GetOrigin()[i])*recip;
            t = (mMinima[i] - segment.GetOrigin()[i])*recip;
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
// @ IvAABB:Classify()
// ---------------------------------------------------------------------------
// Compute signed distance between AABB and plane
//-----------------------------------------------------------------------------
float
IvAABB::Classify( const IvPlane& plane ) const
{
    IvVector3 diagMin, diagMax;
    // set min/max values for x direction
    if ( plane.GetNormal().x >= 0.0f )
    {
        diagMin.x = mMinima.x;
        diagMax.x = mMaxima.x;
    }
    else
    {
        diagMin.x = mMinima.x;
        diagMax.x = mMaxima.x;
    }

    // set min/max values for y direction
    if ( plane.GetNormal().x >= 0.0f )
    {
        diagMin.y = mMinima.y;
        diagMax.y = mMaxima.y;
    }
    else
    {
        diagMin.y = mMinima.y;
        diagMax.y = mMaxima.y;
    }

    // set min/max values for z direction
    if ( plane.GetNormal().z >= 0.0f )
    {
        diagMin.z = mMinima.z;
        diagMax.z = mMaxima.z;
    }
    else
    {
        diagMin.z = mMinima.z;
        diagMax.z = mMaxima.z;
    }

    // minimum on positive side of plane, box on positive side
    float test = plane.Test( diagMin );
    if ( test > 0.0f )
        return test;

    test = plane.Test( diagMax );
    // min on non-positive side, max on non-negative side, intersection
    if ( test >= 0.0f )
        return 0.0f;
    // max on negative side, box on negative side
    else
        return test;

}   // End of AABB::Classify()


//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two AABBs together to create a new one
//-----------------------------------------------------------------------------
void
Merge( IvAABB& result, const IvAABB& b0, const IvAABB& b1 )
{
    IvVector3 newMinima( b0.mMinima );
    IvVector3 newMaxima( b0.mMaxima );

    if (b1.mMinima.x < newMinima.x)
        newMinima.x = b1.mMinima.x;
    if (b1.mMinima.y < newMinima.y)
        newMinima.y = b1.mMinima.y;
    if (b1.mMinima.z < newMinima.z)
        newMinima.z = b1.mMinima.z;

    if (b1.mMaxima.x > newMaxima.x )
        newMaxima.x = b1.mMaxima.x;
    if (b1.mMaxima.y > newMaxima.y )
        newMaxima.y = b1.mMaxima.y;
    if (b1.mMaxima.z > newMaxima.z )
        newMaxima.z = b1.mMaxima.z;

    // set new box
    result.mMinima = newMinima;
    result.mMaxima = newMaxima;

}   // End of ::Merge()
