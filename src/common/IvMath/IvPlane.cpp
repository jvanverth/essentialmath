//----------------------------------------------------------------------------
// @ IvPlane.cpp
// 
// 3D plane class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//----------------------------------------------------------------------------
//-- Includes ----------------------------------------------------------------
//----------------------------------------------------------------------------

#include "IvPlane.h"
#include "IvLine3.h"
#include "IvMath.h"
#include "IvMatrix33.h"
#include "IvQuat.h"
#include "IvVector3.h"
#include "IvAssert.h"

//----------------------------------------------------------------------------
//-- Static Variables --------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//-- Functions ---------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// @ IvPlane::IvPlane()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvPlane::IvPlane() :
    mNormal( 1.0f, 0.0f, 0.0f ),
    mOffset( 0.0f )
{
}   // End of IvPlane::IvPlane()


//----------------------------------------------------------------------------
// @ IvPlane::IvPlane()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvPlane::IvPlane( float a, float b, float c, float d )
{
    Set( a, b, c, d );

}   // End of IvPlane::IvPlane()


//----------------------------------------------------------------------------
// @ IvPlane::IvPlane()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
IvPlane::IvPlane( const IvVector3& p0, const IvVector3& p1, const IvVector3& p2 )
{
    Set( p0, p1, p2 );

}   // End of IvPlane::IvPlane()


//----------------------------------------------------------------------------
// @ IvPlane::IvPlane()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
IvPlane::IvPlane( const IvPlane& other ) :
    mNormal( other.mNormal ),
    mOffset( other.mOffset )
{

}   // End of IvPlane::IvPlane()


//----------------------------------------------------------------------------
// @ IvPlane::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
IvPlane&
IvPlane::operator=( const IvPlane& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mNormal = other.mNormal;
    mOffset = other.mOffset;

    return *this;

}   // End of IvPlane::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text IvPlane.  The format is assumed to be :
// [ IvVector3, IvVector3 ]
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvPlane& source)
{
    return out << "[" << source.mNormal 
                     << ", " << source.mOffset << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ IvPlane::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
IvPlane::Get( IvVector3& normal, float& offset ) const
{
    normal = mNormal;
    offset = mOffset;

}   // End of IvPlane::Get()


//----------------------------------------------------------------------------
// @ IvPlane::operator==()
// ---------------------------------------------------------------------------
// Are two IvPlane's equal?
//----------------------------------------------------------------------------
bool
IvPlane::operator==( const IvPlane& plane ) const
{
    return (plane.mNormal == mNormal && plane.mOffset == mOffset);

}  // End of IvPlane::operator==()


//----------------------------------------------------------------------------
// @ IvPlane::operator!=()
// ---------------------------------------------------------------------------
// Are two IvPlane's not equal?
//----------------------------------------------------------------------------
bool
IvPlane::operator!=( const IvPlane& plane ) const
{
    return !(plane.mNormal == mNormal && plane.mOffset == mOffset);
}  // End of IvPlane::operator!=()


//----------------------------------------------------------------------------
// @ IvPlane::Set()
// ---------------------------------------------------------------------------
// Sets the parameters
//-----------------------------------------------------------------------------
void
IvPlane::Set( float a, float b, float c, float d )
{
    // normalize for cheap distance checks
    float lensq = a*a + b*b + c*c;
    // length of normal had better not be zero
    ASSERT( !IvIsZero( lensq ) );

    // recover gracefully
    if ( IvIsZero( lensq ) )
    {
        mNormal = IvVector3::xAxis;
        mOffset = 0.0f;
    }
    else
    {
        float recip = IvRecipSqrt(lensq);
        mNormal.Set( a*recip, b*recip, c*recip );
        mOffset = d*recip;
    }

}   // End of IvPlane::Set()


//----------------------------------------------------------------------------
// @ IvPlane::Set()
// ---------------------------------------------------------------------------
// Sets the parameters
//-----------------------------------------------------------------------------
void 
IvPlane::Set( const IvVector3& p0, const IvVector3& p1, const IvVector3& p2 )
{
    // get plane vectors
    IvVector3 u = p1 - p0;
    IvVector3 v = p2 - p0;
    IvVector3 w = u.Cross(v);
    
    // normalize for cheap distance checks
    float lensq = w.x*w.x + w.y*w.y + w.z*w.z;
    // length of normal had better not be zero
    ASSERT( !IvIsZero( lensq ) );

    // recover gracefully
    if ( IvIsZero( lensq ) )
    {
        mNormal = IvVector3::xAxis;
        mOffset = 0.0f;
    }
    else
    {
        float recip = 1.0f/lensq;
        mNormal.Set( w.x*recip, w.y*recip, w.z*recip );
        mOffset = -mNormal.Dot(p0);
    }

}   // End of IvPlane::Set()


//----------------------------------------------------------------------------
// @ IvPlane::Transform()
// ---------------------------------------------------------------------------
// Transforms plane into new space
//-----------------------------------------------------------------------------
IvPlane 
IvPlane::Transform( float scale, const IvQuat& rotate, const IvVector3& translate ) const
{
    IvPlane plane;

    // get rotation matrix
    IvMatrix33    rotmatrix(rotate);

    // transform to get normal
    plane.mNormal = rotmatrix*mNormal/scale;
    
    // transform to get offset
    IvVector3 newTrans = translate*rotmatrix;
    plane.mOffset = -newTrans.Dot( mNormal )/scale + mOffset;

    return plane;

}   // End of IvPlane::Transform()


//----------------------------------------------------------------------------
// @ IvPlane::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on plane to point
//-----------------------------------------------------------------------------
IvVector3 
IvPlane::ClosestPoint( const IvVector3& point ) const
{    
    return point - Test(point)*mNormal; 

}   // End of IvPlane::ClosestPoint()

