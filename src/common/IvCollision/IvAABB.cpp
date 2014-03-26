//===============================================================================
// @ IvAABB.cpp
// 
// Axis-aligned bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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
        if (points[i].GetX() < mMinima.GetX())
            mMinima.SetX( points[i].GetX() );
        else if (points[i].GetX() > mMaxima.GetX() )
            mMaxima.SetX( points[i].GetX() );
        if (points[i].GetY() < mMinima.GetY())
            mMinima.SetY( points[i].GetY() );
        else if (points[i].GetY() > mMaxima.GetY() )
            mMaxima.SetY( points[i].GetY() );
        if (points[i].GetZ() < mMinima.GetZ())
            mMinima.SetZ( points[i].GetZ() );
        else if (points[i].GetZ() > mMaxima.GetZ() )
            mMaxima.SetZ( points[i].GetZ() );
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
    if (point.GetX() < mMinima.GetX())
        mMinima.SetX( point.GetX() );
    else if (point.GetX() > mMaxima.GetX() )
        mMaxima.SetX( point.GetX() );
    if (point.GetY() < mMinima.GetY())
        mMinima.SetY( point.GetY() );
    else if (point.GetY() > mMaxima.GetY() )
        mMaxima.SetY( point.GetY() );
    if (point.GetZ() < mMinima.GetZ())
        mMinima.SetZ( point.GetZ() );
    else if (point.GetZ() > mMaxima.GetZ() )
        mMaxima.SetZ( point.GetZ() );

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
    if (mMinima.GetX() > other.mMaxima.GetX() || other.mMinima.GetX() > mMaxima.GetX() )
        return false;

    // if separated in y direction
    if (mMinima.GetY() > other.mMaxima.GetY() || other.mMinima.GetY() > mMaxima.GetY() )
        return false;

    // if separated in z direction
    if (mMinima.GetZ() > other.mMaxima.GetZ() || other.mMinima.GetZ() > mMaxima.GetZ() )
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
        // line is parallel to plane
        if ( ::IsZero( line.GetDirection()[i] ) )
        {
            // line passes by box
            if ( (line.GetOrigin())[i] < mMinima[i] || (line.GetOrigin())[i] > mMaxima[i] )
                return false;
        }
        else
        {
            // compute intersection parameters and sort
            float s = (mMinima[i] - line.GetOrigin()[i])/line.GetDirection()[i];
            float t = (mMaxima[i] - line.GetOrigin()[i])/line.GetDirection()[i];
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
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // ray is parallel to plane
        if ( ::IsZero( ray.GetDirection()[i] ) )
        {
            // ray passes by box
            if ( (ray.GetOrigin())[i] < mMinima[i] || (ray.GetOrigin())[i] > mMaxima[i] )
                return false;
        }
        else
        {
            // compute intersection parameters and sort
            float s = (mMinima[i] - ray.GetOrigin()[i])/ray.GetDirection()[i];
            float t = (mMaxima[i] - ray.GetOrigin()[i])/ray.GetDirection()[i];
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
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // segment is parallel to plane
        if ( ::IsZero( segment.GetDirection()[i] ) )
        {
            // segment passes by box
            if ( (segment.GetOrigin())[i] < mMinima[i] || (segment.GetOrigin())[i] > mMaxima[i] )
                return false;
        }
        else
        {
            // compute intersection parameters and sort
            float s = (mMinima[i] - segment.GetOrigin()[i])/segment.GetDirection()[i];
            float t = (mMaxima[i] - segment.GetOrigin()[i])/segment.GetDirection()[i];
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
    if ( plane.GetNormal().GetX() >= 0.0f )
    {
        diagMin.SetX( mMinima.GetX() );
        diagMax.SetX( mMaxima.GetX() );
    }
    else
    {
        diagMin.SetX( mMinima.GetX() );
        diagMax.SetX( mMaxima.GetX() );
    }

    // set min/max values for y direction
    if ( plane.GetNormal().GetX() >= 0.0f )
    {
        diagMin.SetY( mMinima.GetY() );
        diagMax.SetY( mMaxima.GetY() );
    }
    else
    {
        diagMin.SetY( mMinima.GetY() );
        diagMax.SetY( mMaxima.GetY() );
    }

    // set min/max values for z direction
    if ( plane.GetNormal().GetZ() >= 0.0f )
    {
        diagMin.SetZ( mMinima.GetZ() );
        diagMax.SetZ( mMaxima.GetZ() );
    }
    else
    {
        diagMin.SetZ( mMinima.GetZ() );
        diagMax.SetZ( mMaxima.GetZ() );
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

    if (b1.mMinima.GetX() < newMinima.GetX())
        newMinima.SetX( b1.mMinima.GetX() );
    if (b1.mMinima.GetY() < newMinima.GetY())
        newMinima.SetY( b1.mMinima.GetY() );
    if (b1.mMinima.GetZ() < newMinima.GetZ())
        newMinima.SetZ( b1.mMinima.GetZ() );

    if (b1.mMaxima.GetX() > newMaxima.GetX() )
        newMaxima.SetX( b1.mMaxima.GetX() );
    if (b1.mMaxima.GetY() > newMaxima.GetY() )
        newMaxima.SetY( b1.mMaxima.GetY() );
    if (b1.mMaxima.GetZ() > newMaxima.GetZ() )
        newMaxima.SetZ( b1.mMaxima.GetZ() );

    // set new box
    result.mMinima = newMinima;
    result.mMaxima = newMaxima;

}   // End of ::Merge()
