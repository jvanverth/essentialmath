//===============================================================================
// @ IvBezier.cpp
// 
// Piecewise Bezier spline
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvBezier.h"
#include <IvAssert.h>
#include <IvRendererHelp.h>
#include <IvRenderer.h>
#include <IvResourceManager.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>
#include <IvMath.h>
#include <IvLineSegment3.h>
#include <float.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvBezier::IvBezier()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvBezier::IvBezier() :
    mPositions( 0 ),
    mControls( 0 ),
    mTimes( 0 ),
    mLengths( 0 ),
    mTotalLength( 0.0f ),
    mCount( 0 ),
    mCurveVertices( 0 ),
    mPointVertices( 0 ),
    mControlVertices( 0 )
{
}   // End of IvBezier::IvBezier()


//-------------------------------------------------------------------------------
// @ IvBezier::IvBezier()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvBezier::~IvBezier()
{
    Clean();

}   // End of IvBezier::~IvBezier()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvBezier& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
        if ( i < source.mCount-1 )
            out << source.mControls[2*i] << ',' << source.mControls[2*i+1] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ IvBezier::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
IvBezier::Initialize( const IvVector3* positions,
                       const IvVector3* controls,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !positions || !times || !controls )
        return false;

    // set up arrays
    mPositions = new IvVector3[count];
    mControls = new IvVector3[2*(count-1)];
    mTimes = new float[count];
    mCount = count;

    // copy interpolant data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        mTimes[i] = times[i];
    }

    // copy approximating control points
    for ( i = 0; i < 2*(count-1); ++i )
    {
        mControls[i] = controls[i];
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return RebuildVertexBuffers();

}   // End of IvBezier::Initialize()


//-------------------------------------------------------------------------------
// @ IvBezier::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
IvBezier::Initialize( const IvVector3* positions,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !positions || !times )
        return false;

    // set up arrays
    mPositions = new IvVector3[count];
    mControls = new IvVector3[2*(count-1)];
    mTimes = new float[count];
    mCount = count;

    // copy interpolant data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        mTimes[i] = times[i];
    }

    // create approximating control points
    for ( i = 0; i < count-1; ++i )
    {
        if ( i > 0 )
            mControls[2*i] = mPositions[i] + (mPositions[i+1]-mPositions[i-1])/3.0f;
        if ( i < count-2 )
            mControls[2*i+1] = mPositions[i+1] - (mPositions[i+2]-mPositions[i])/3.0f;
    }
    mControls[0] = mControls[1] - (mPositions[1] - mPositions[0])/3.0f;
    mControls[2*count-3] = 
            mControls[2*count-4] + (mPositions[count-1] - mPositions[count-2])/3.0f;

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return true;

}   // End of IvBezier::Initialize()


//-------------------------------------------------------------------------------
// @ IvBezier::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void IvBezier::Clean()
{
    delete [] mPositions;
    delete [] mControls;
    delete [] mTimes;
    delete [] mLengths;
    mTotalLength = 0.0f;
    mCount = 0;

}   // End of IvBezier::Clean()


//-------------------------------------------------------------------------------
// @ IvBezier::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
IvVector3
IvBezier::Evaluate( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mPositions[0];
    else if ( t >= mTimes[mCount-1] )
        return mPositions[mCount-1];

    // find segment and parameter
    unsigned int i;
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t < mTimes[i+1] )
        {
            break;
        }
    }
    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // evaluate
    IvVector3 A = mPositions[i+1]
                - 3.0f*mControls[2*i+1]
                + 3.0f*mControls[2*i]
                - mPositions[i];
    IvVector3 B = 3.0f*mControls[2*i+1]
                - 6.0f*mControls[2*i]
                + 3.0f*mPositions[i];
    IvVector3 C = 3.0f*mControls[2*i]
                - 3.0f*mPositions[i];
    
    return mPositions[i] + u*(C + u*(B + u*A));

}   // End of IvBezier::Evaluate()


//-------------------------------------------------------------------------------
// @ IvBezier::Velocity()
//-------------------------------------------------------------------------------
// Evaluate spline's derivative
//-------------------------------------------------------------------------------
IvVector3
IvBezier::Velocity( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mPositions[0];
    else if ( t >= mTimes[mCount-1] )
        return mPositions[mCount-1];

    // find segment and parameter
    unsigned int i;
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t < mTimes[i+1] )
        {
            break;
        }
    }
    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // evaluate
    IvVector3 A = mPositions[i+1]
                - 3.0f*mControls[2*i+1]
                + 3.0f*mControls[2*i]
                - mPositions[i];
    IvVector3 B = 6.0f*mControls[2*i+1]
                - 12.0f*mControls[2*i]
                + 6.0f*mPositions[i];
    IvVector3 C = 3.0f*mControls[2*i]
                - 3.0f*mPositions[i];
    
    return C + u*(B + 3.0f*u*A);

}   // End of IvBezier::Velocity()


//-------------------------------------------------------------------------------
// @ IvBezier::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate spline's second derivative
//-------------------------------------------------------------------------------
IvVector3
IvBezier::Acceleration( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mPositions[0];
    else if ( t >= mTimes[mCount-1] )
        return mPositions[mCount-1];

    // find segment and parameter
    unsigned int i;
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t < mTimes[i+1] )
        {
            break;
        }
    }
    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // evaluate
    IvVector3 A = mPositions[i+1]
                - 3.0f*mControls[2*i+1]
                + 3.0f*mControls[2*i]
                - mPositions[i];
    IvVector3 B = 6.0f*mControls[2*i+1]
                - 12.0f*mControls[2*i]
                + 6.0f*mPositions[i];
    
    return B + 6.0f*u*A;

}   // End of IvBezier::Acceleration()


//-------------------------------------------------------------------------------
// @ IvBezier::FindParameterByDistance()
//-------------------------------------------------------------------------------
// Find parameter s distance in arc length from Q(t1)
// Returns max float if can't find it
//
// This extends the approach in the text and uses a mixture of bisection and 
// Newton-Raphson to find the root.  The result is more stable than Newton-
// Raphson alone because a) we won't end up with a situation where we divide by 
// zero in the Newton-Raphson step and b) the end result converges faster.
//
// See Numerical Recipes or http://www.essentialmath.com/blog for more details.
//-------------------------------------------------------------------------------
float 
IvBezier::FindParameterByDistance( float t1, float s )
{
    // initialize bisection endpoints
    float a = t1;
    float b = mTimes[mCount-1];

    // ensure that we remain within valid parameter space
    if ( s >= ArcLength(t1, b) )
        return b;
    if ( s <= 0.0f )
        return a;

    // make first guess
    float p = t1 + s*(mTimes[mCount-1]-mTimes[0])/mTotalLength;

    // iterate and look for zeros
    for ( UInt32 i = 0; i < 32; ++i )
    {
        // compute function value and test against zero
        float func = ArcLength(t1, p) - s;
        if ( ::IvAbs(func) < 1.0e-03f )
        {
            return p;
        }

         // update bisection endpoints
        if ( func < 0.0f )
        {
            a = p;
        }
        else
        {
            b = p;
        }

        // get speed along curve
        float speed = Velocity(p).Length();

        // if result will lie outside [a,b] 
        if ( ((p-a)*speed - func)*((p-b)*speed - func) > -1.0e-3f )
        {
            // do bisection
            p = 0.5f*(a+b);
        }    
        else
        {
            // otherwise Newton-Raphson
            p -= func/speed;
        }
    }

    // done iterating, return failure case
    return FLT_MAX;

}   // End of IvBezier::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ IvBezier::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
IvBezier::ArcLength( float t1, float t2 )
{
    if ( t2 <= t1 )
        return 0.0f;

    if ( t1 < mTimes[0] )
        t1 = mTimes[0];

    if ( t2 > mTimes[mCount-1] )
        t2 = mTimes[mCount-1];

    // find segment and parameter
    unsigned int seg1;
    for ( seg1 = 0; seg1 < mCount-1; ++seg1 )
    {
        if ( t1 < mTimes[seg1+1] )
        {
            break;
        }
    }
    float u1 = (t1 - mTimes[seg1])/(mTimes[seg1+1] - mTimes[seg1]);
    
    // find segment and parameter
    unsigned int seg2;
    for ( seg2 = 0; seg2 < mCount-1; ++seg2 )
    {
        if ( t2 <= mTimes[seg2+1] )
        {
            break;
        }
    }
    float u2 = (t2 - mTimes[seg2])/(mTimes[seg2+1] - mTimes[seg2]);
    
    float result;
    // both parameters lie in one segment
    if ( seg1 == seg2 )
    {
        result = SegmentArcLength( seg1, u1, u2 );
    }
    // parameters cross segments
    else
    {
        result = SegmentArcLength( seg1, u1, 1.0f );
        for ( UInt32 i = seg1+1; i < seg2; ++i )
            result += mLengths[i];
        result += SegmentArcLength( seg2, 0.0f, u2 );
    }

    return result;

}   // End of IvBezier::ArcLength()


//-------------------------------------------------------------------------------
// @ IvBezier::SegmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
IvBezier::SegmentArcLength( UInt32 i, float u1, float u2 )
{
    ASSERT(i >= 0 && i < mCount-1);

    if ( u2 <= u1 )
        return 0.0f;

    if ( u1 < 0.0f )
        u1 = 0.0f;

    if ( u2 > 1.0f )
        u2 = 1.0f;

    IvVector3 P0 = mPositions[i];
    IvVector3 P1 = mControls[2*i];
    IvVector3 P2 = mControls[2*i+1];
    IvVector3 P3 = mPositions[i+1];

    // get control points for subcurve from 0.0 to u2 (de Casteljau's method)
    float minus_u2 = (1.0f - u2);
    IvVector3 L1 = minus_u2*P0 + u2*P1;
    IvVector3 H = minus_u2*P1 + u2*P2;
    IvVector3 L2 = minus_u2*L1 + u2*H;
    IvVector3 L3 = minus_u2*L2 + u2*(minus_u2*H + u2*(minus_u2*P2 + u2*P3));

    // resubdivide to get control points for subcurve between u1 and u2
    float minus_u1 = (1.0f - u1);
    H = minus_u1*L1 + u1*L2;
    IvVector3 R3 = L3;
    IvVector3 R2 = minus_u1*L2 + u1*L3;
    IvVector3 R1 = minus_u1*H + u1*R2;
    IvVector3 R0 = minus_u1*(minus_u1*(minus_u1*P0 + u1*L1) + u1*H) + u1*R1;

    // get length through subdivision
    return SubdivideLength( R0, R1, R2, R3 );

}   // End of IvBezier::SegmentArcLength()


//-------------------------------------------------------------------------------
// @ IvBezier::SubdivideLength()
//-------------------------------------------------------------------------------
// Get length of Bezier curve using midpoint subdivision
//-------------------------------------------------------------------------------
float 
IvBezier::SubdivideLength( const IvVector3& P0, const IvVector3& P1, 
                     const IvVector3& P2, const IvVector3& P3 )
{
    // check to see if basically straight
    float Lmin = ::Distance( P0, P3 );
    float Lmax = ::Distance( P0, P1 ) + ::Distance( P1, P2 ) + ::Distance( P2, P3 );
    float diff = Lmin - Lmax;

    if ( diff*diff < 1.0e-3f )
        return 0.5f*(Lmin + Lmax);

    // otherwise get control points for subdivision
    IvVector3 L1 = (P0 + P1) * 0.5f;
    IvVector3 H = (P1 + P2) * 0.5f;
    IvVector3 L2 = (L1 + H) * 0.5f;
    IvVector3 R2 = (P2 + P3) * 0.5f;
    IvVector3 R1 = (H + R2) * 0.5f;
    IvVector3 mid = (L2 + R1) * 0.5f;

    // subdivide
    return SubdivideLength( P0, L1, L2, mid ) + SubdivideLength( mid, R1, R2, P3 );

}   // End of IvBezier::SubdivideLength()


//-------------------------------------------------------------------------------
// @ IvBezier::RebuildVertexBuffers()
//-------------------------------------------------------------------------------
// Build spline vertex buffer data
// Uses midpoint subdivision
//-------------------------------------------------------------------------------
bool 
IvBezier::RebuildVertexBuffers()
{
    // build Catmull-Rom spline

    // make sure the vertex buffer is appropriate for current curve data

    // count verts
    UInt32 numverts = 1;    // for final vertex

    // subdivide to get all but last point
    for ( UInt32 i = 0; i < mCount-1; ++i )
    {
        numverts += CountSubdivideVerts( mPositions[i], mControls[2*i], mControls[2*i+1], mPositions[i+1] );
    }

    if ( mCurveVertices && mCurveVertices->GetVertexCount() != numverts )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCurveVertices);
        mCurveVertices = 0;
    }

    if ( mCurveVertices == 0 )
    {
        mCurveVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, numverts);
    }

	IvCPVertex* curveDataPtr = (IvCPVertex*) mCurveVertices->BeginLoadData();

    // subdivide to get all but last point
    for ( unsigned int i = 0; i < mCount-1; ++i )
    {
        curveDataPtr = SubdivideCurve( curveDataPtr, mPositions[i], mControls[2*i], mControls[2*i+1], mPositions[i+1] );
    }

    // output last point
    curveDataPtr->position = mPositions[mCount-1];
    curveDataPtr->color = kOrange;

	if (!mCurveVertices->EndLoadData())
        return false;

    // set up sample points
    // make sure the vertex buffer is appropriate for current curve data
    if ( mPointVertices && mPointVertices->GetVertexCount() != mCount )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mPointVertices);
        mPointVertices = 0;
    }

    if ( mPointVertices == 0 )
    {
        mPointVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, mCount);
    }

	IvCPVertex *pointDataPtr = (IvCPVertex*) mPointVertices->BeginLoadData();
    
    for ( UInt32 i = 0; i < mCount; ++i )
    {
        pointDataPtr[i].position = mPositions[i];
        pointDataPtr[i].color = kRed;
    }
    if (!mPointVertices->EndLoadData())
        return false;

    // set up control points
    // make sure the vertex buffer is appropriate for current curve data
    if ( mControlVertices && mControlVertices->GetVertexCount() != 2*(mCount-1) )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mControlVertices);
        mControlVertices = 0;
    }

    if ( mControlVertices == 0 )
    {
        mControlVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 2*(mCount-1));
    }

	IvCPVertex *controlDataPtr = (IvCPVertex*) mControlVertices->BeginLoadData();
    
    for ( UInt32 i = 0; i < 2*(mCount-1); ++i )
    {
        controlDataPtr[i].position = mControls[i];
        controlDataPtr[i].color = kYellow;
    }
    if (!mControlVertices->EndLoadData())
        return false;

    return true;

}   // End of IvBezier::Render()

//-------------------------------------------------------------------------------
// @ IvBezier::CountSubdivideVerts()
//-------------------------------------------------------------------------------
// Count number of points for Bezier curve using midpoint subdivision
//-------------------------------------------------------------------------------
unsigned int 
IvBezier::CountSubdivideVerts( const IvVector3& P0, const IvVector3& P1, 
                     const IvVector3& P2, const IvVector3& P3 )
{
    // check to see if straight
    IvLineSegment3 segment( P0, P3 );
    float t;
    if ( ::DistanceSquared( segment, P1, t ) < 1.0e-6f &&
         ::DistanceSquared( segment, P2, t ) < 1.0e-6f )
         return 1;

    // otherwise get control points for subdivision
    IvVector3 L1 = (P0 + P1) * 0.5f;
    IvVector3 H = (P1 + P2) * 0.5f;
    IvVector3 L2 = (L1 + H) * 0.5f;
    IvVector3 R2 = (P2 + P3) * 0.5f;
    IvVector3 R1 = (H + R2) * 0.5f;
    IvVector3 mid = (L2 + R1) * 0.5f;

    // subdivide
    return 1 + CountSubdivideVerts( P0, L1, L2, mid ) + CountSubdivideVerts( mid, R1, R2, P3 );

}   // End of IvBezier::CountSubdivideVerts()


//-------------------------------------------------------------------------------
// @ IvBezier::SubdivideCurve()
//-------------------------------------------------------------------------------
// Output points for Bezier curve using midpoint subdivision
//-------------------------------------------------------------------------------
IvCPVertex* 
IvBezier::SubdivideCurve( IvCPVertex* currentVertex, const IvVector3& P0, const IvVector3& P1, 
                     const IvVector3& P2, const IvVector3& P3 )
{
    // output first point
    currentVertex->position = P0;
    currentVertex->color = kOrange;
    ++currentVertex;

    // check to see if straight
    IvLineSegment3 segment( P0, P3 );
    float t;
    if ( ::DistanceSquared( segment, P1, t ) < 1.0e-6f &&
         ::DistanceSquared( segment, P2, t ) < 1.0e-6f )
         return currentVertex;

    // otherwise get control points for subdivision
    IvVector3 L1 = (P0 + P1) * 0.5f;
    IvVector3 H = (P1 + P2) * 0.5f;
    IvVector3 L2 = (L1 + H) * 0.5f;
    IvVector3 R2 = (P2 + P3) * 0.5f;
    IvVector3 R1 = (H + R2) * 0.5f;
    IvVector3 mid = (L2 + R1) * 0.5f;

    // subdivide
    currentVertex = SubdivideCurve( currentVertex, P0, L1, L2, mid );
    currentVertex = SubdivideCurve( currentVertex, mid, R1, R2, P3 );

    return currentVertex;

}   // End of IvBezier::SubdivideCurve()


//-------------------------------------------------------------------------------
// @ IvBezier::Render()
//-------------------------------------------------------------------------------
// Render spline
//-------------------------------------------------------------------------------
void 
IvBezier::Render()
{
    IvRenderer::mRenderer->Draw( kLineStripPrim, mCurveVertices );
    IvRenderer::mRenderer->Draw( kPointListPrim, mPointVertices );
    IvRenderer::mRenderer->Draw( kPointListPrim, mControlVertices );

}   // End of IvBezier::Render()


