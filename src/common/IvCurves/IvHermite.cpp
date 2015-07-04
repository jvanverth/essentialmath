//===============================================================================
// @ IvHermite.cpp
// 
// Hermite spline
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvHermite.h"
#include <IvAssert.h>
#include <IvRendererHelp.h>
#include <IvGaussianElim.h>
#include <IvRenderer.h>
#include <IvResourceManager.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>
#include <IvMath.h>
#include <string.h>
#include <float.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvHermite::IvHermite()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvHermite::IvHermite() :
    mPositions( 0 ),
    mInTangents( 0 ),
    mOutTangents( 0 ),
    mTimes( 0 ),
    mLengths( 0 ),
    mTotalLength( 0.0f ),
    mCount( 0 ),
    mCurveVertices( 0 ),
    mPointVertices( 0 ),
    mTangentVertices( 0 )
{
}   // End of IvHermite::IvHermite()


//-------------------------------------------------------------------------------
// @ IvHermite::IvHermite()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvHermite::~IvHermite()
{
    Clean();

}   // End of IvHermite::~IvHermite()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvHermite& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i];
        if ( i < source.mCount-1 )
            out << ',' << source.mInTangents[i];
        if ( i > 0 )
            out << ',' << source.mOutTangents[i-1];
        out << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ IvHermite::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
IvHermite::Initialize( const IvVector3* positions,
                       const IvVector3* inTangents,
                       const IvVector3* outTangents,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !positions || !times || !inTangents || !outTangents )
        return false;

    // set up arrays
    mPositions = new IvVector3[count];
    mInTangents = new IvVector3[count-1];
    mOutTangents = new IvVector3[count-1];
    mTimes = new float[count];
    mCount = count;

    // copy data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        if ( i < count-1 )
        {
            mInTangents[i] = inTangents[i];
            mOutTangents[i] = outTangents[i];
        }
        mTimes[i] = times[i];
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    (void) RebuildVertexBuffers();

    return true;

}   // End of IvHermite::Initialize()


//-------------------------------------------------------------------------------
// @ IvHermite::InitializeClamped()
//-------------------------------------------------------------------------------
// Set up sample points for clamped spline
//-------------------------------------------------------------------------------
bool 
IvHermite::InitializeClamped( const IvVector3* positions, 
                              const float* times,
                              unsigned int count,
                              const IvVector3& inTangent,
                              const IvVector3& outTangent )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 3 || !positions || !times )
        return false;

    // build A
    unsigned int n = count;
    float* A = new float[n*n];
    memset(A, 0, sizeof(float)*n*n);

    A[0] = 1.0f;
    unsigned int i;
    for ( i = 1; i < n-1; ++i )
    {
        A[i + n*i - n] = 1.0f;
        A[i + n*i] = 4.0f;
        A[i + n*i + n] = 1.0f;
    }
    A[n*n-1] = 1.0f;
    
    // invert it
    // we'd might get better accuracy if we solve the linear system 3 times,
    // once each for x, y, and z, but this is more efficient
    if (!::InvertMatrix( A, n ))
    {
        delete [] A;
        return false;
    }

    // set up arrays
    mPositions = new IvVector3[count];
    mInTangents = new IvVector3[count-1];
    mOutTangents = new IvVector3[count-1];
    mTimes = new float[count];
    mCount = count;

    // handle end conditions
    mPositions[0] = positions[0];
    mTimes[0] = times[0];
    mOutTangents[0] = outTangent;
    mPositions[count-1] = positions[count-1];
    mTimes[count-1] = times[count-1];
    mInTangents[count-2] = inTangent;

    // set up the middle
    for ( i = 1; i < count-1; ++i )
    {
        // copy position and time
        mPositions[i] = positions[i];
        mTimes[i] = times[i];

        // multiply b by inverse of A to get x
        mOutTangents[i] = A[i]*outTangent + A[i + n*n-n]*inTangent;
        for ( UInt32 j = 1; j < n-1; ++j )
        {
            IvVector3 b_j = 3.0f*(positions[j+1]-positions[j-1]);
            mOutTangents[i] += A[i + n*j]*b_j;
        }

        // in tangent is out tangent of next segment
        mInTangents[i-1] = mOutTangents[i];
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    delete [] A;

    (void) RebuildVertexBuffers();

    return true;

}   // End of IvHermite::InitializeClamped()


//-------------------------------------------------------------------------------
// @ IvHermite::InitializeNatural()
//-------------------------------------------------------------------------------
// Set up sample points for natural spline
//
// Uses tridiagonal matrix solver
//-------------------------------------------------------------------------------
bool 
IvHermite::InitializeNatural( const IvVector3* positions, 
                              const float* times,
                              unsigned int count )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 3 || !positions || !times )
        return false;

    // set up arrays
    mPositions = new IvVector3[count];
    mInTangents = new IvVector3[count-1];
    mOutTangents = new IvVector3[count-1];
    mTimes = new float[count];
    mCount = count;

    // copy positions and times
    for ( unsigned int i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        mTimes[i] = times[i];
    }

    // build tangent data
    unsigned int n = count;
    float L;                          // current lower diagonal matrix entry
    float* U = new float[n];          // upper diagonal matrix entries
    IvVector3* z = new IvVector3[n];  // solution of lower diagonal system Lz = b

    // solve for upper matrix and partial solution z
    L = 2.0f;
    U[0] = 0.5f;
    z[0] = 3.0f*(positions[1] - positions[0])/L;
    for ( unsigned int i = 1; i < n-1; ++i )
    {
        // add internal entry to linear system for smooth spline
        L = 4.0f - U[i-1];
        U[i] = 1.0f/L;
        z[i] = 3.0f*(positions[i+1] - positions[i-1]);
        z[i] -= z[i-1];
        z[i] /= L;
    }
    L = 2.0f - U[n-2];
    z[n-1] = 3.0f*(positions[n-1] - positions[n-2]);
    z[n-1] -= z[n-2];
    z[n-1] /= L;

    // solve Ux = z (see Burden and Faires for details)
    mInTangents[n-2] = z[n-1];
    for ( unsigned int i = n-2; i > 0; --i )
    {
        mInTangents[i-1] = z[i] - U[i]*mInTangents[i];
        mOutTangents[i] = mInTangents[i-1];
    }
    mOutTangents[0] = z[0] - U[0]*mInTangents[0];

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( unsigned int i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    delete [] U;
    delete [] z;

    (void) RebuildVertexBuffers();

    return true;

}   // End of IvHermite::InitializeNatural()


//-------------------------------------------------------------------------------
// @ IvHermite::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void IvHermite::Clean()
{
    delete [] mPositions;
    delete [] mInTangents;
    delete [] mOutTangents;
    delete [] mTimes;
    delete [] mLengths;
    mTotalLength = 0.0f;
    mCount = 0;

}   // End of IvHermite::Clean()


//-------------------------------------------------------------------------------
// @ IvHermite::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
IvVector3
IvHermite::Evaluate( float t )
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
    IvVector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    IvVector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - mInTangents[i]
                - 2.0f*mOutTangents[i];
    
    return mPositions[i] + u*(mOutTangents[i] + u*(B + u*A));

}   // End of IvHermite::Evaluate()


//-------------------------------------------------------------------------------
// @ IvHermite::Velocity()
//-------------------------------------------------------------------------------
// Evaluate derivative at parameter t
//-------------------------------------------------------------------------------
IvVector3
IvHermite::Velocity( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mOutTangents[0];
    else if ( t >= mTimes[mCount-1] )
        return mInTangents[mCount-2];

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
    IvVector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    IvVector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - mInTangents[i]
                - 2.0f*mOutTangents[i];
    
    return mOutTangents[i] + u*(2.0f*B + 3.0f*u*A);

}   // End of IvHermite::Velocity()


//-------------------------------------------------------------------------------
// @ IvHermite::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate second derivative at parameter t
//-------------------------------------------------------------------------------
IvVector3
IvHermite::Acceleration( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        t = 0.0f;
    else if ( t > mTimes[mCount-1] )
        t = mTimes[mCount-1];

    // find segment and parameter
    unsigned int i;
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t <= mTimes[i+1] )
        {
            break;
        }
    }
    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // evaluate
    IvVector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    IvVector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - mInTangents[i]
                - 2.0f*mOutTangents[i];
    
    return 2.0f*B + 6.0f*u*A;

}   // End of IvHermite::Acceleration()


//-------------------------------------------------------------------------------
// @ IvHermite::FindParameterByDistance()
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
IvHermite::FindParameterByDistance( float t1, float s )
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

}   // End of IvHermite::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ IvHermite::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
IvHermite::ArcLength( float t1, float t2 )
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

}   // End of IvHermite::ArcLength()


//-------------------------------------------------------------------------------
// @ IvHermite::SegmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
IvHermite::SegmentArcLength( UInt32 i, float u1, float u2 )
{
    static const float x[] =
    {
        0.0000000000f, 0.5384693101f, -0.5384693101f, 0.9061798459f, -0.9061798459f 
    };

    static const float c[] =
    {
        0.5688888889f, 0.4786286705f, 0.4786286705f, 0.2369268850f, 0.2369268850f
    };

    ASSERT(i >= 0 && i < mCount-1);

    if ( u2 <= u1 )
        return 0.0f;

    if ( u1 < 0.0f )
        u1 = 0.0f;

    if ( u2 > 1.0f )
        u2 = 1.0f;

    // use Gaussian quadrature
    float sum = 0.0f;
    // set up for computation of IvHermite derivative
    IvVector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    IvVector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - mInTangents[i]
                - 2.0f*mOutTangents[i];
    IvVector3 C = mInTangents[i];
    
    for ( UInt32 j = 0; j < 5; ++j )
    {
        float u = 0.5f*((u2 - u1)*x[j] + u2 + u1);
        IvVector3 derivative = C + u*(2.0f*B + 3.0f*u*A);
        sum += c[j]*derivative.Length();
    }
    sum *= 0.5f*(u2-u1);

    return sum;

}   // End of IvHermite::SegmentArcLength()


//-------------------------------------------------------------------------------
// @ IvHermite::RebuildVertexBuffers()
//-------------------------------------------------------------------------------
// Rebuilds vertex buffer rendering data for newly created spline
// Uses forward differencing
//-------------------------------------------------------------------------------
bool 
IvHermite::RebuildVertexBuffers()
{
    const UInt32 vertsPerSegment = 11;
    // build hermite spline

    // make sure the vertex buffer is appropriate for current curve data
    if ( mCurveVertices && mCurveVertices->GetVertexCount() != vertsPerSegment*(mCount-1) )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCurveVertices);
        mCurveVertices = 0;
    }

    if ( mCurveVertices == 0 )
    {
        mCurveVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat,
                                                                                         vertsPerSegment*(mCount-1),
                                                                                         NULL, kDefaultUsage);
    }

	IvCPVertex* curveDataPtr = (IvCPVertex*) mCurveVertices->BeginLoadData();
    UInt32 currentVertex = 0;

    for ( UInt32 i = 0; i < mCount-1; ++i )
    {
        // set up step variables
        const float h = 1.0f/(float)(vertsPerSegment-1);
        const float h2 = h*h;
        const float h3 = h2*h;
        IvVector3 A = 2.0f*mPositions[i]
                    - 2.0f*mPositions[i+1]
                    + mInTangents[i]
                    + mOutTangents[i];
        IvVector3 B = -3.0f*mPositions[i]
                    + 3.0f*mPositions[i+1]
                    - mInTangents[i]
                    - 2.0f*mOutTangents[i];
        IvVector3 dP1 = h3*A + h2*B + h*mOutTangents[i];
        IvVector3 dP3 = 6.0f*h3*A;
        IvVector3 dP2 = dP3 + 2.0f*h2*B;

        // output first point
        IvVector3 P = mPositions[i];
        curveDataPtr[currentVertex].position = P;
        curveDataPtr[currentVertex++].color = kOrange;

        // do forward differencing to next points
        for ( UInt32 j = 1; j < vertsPerSegment; ++j )
        {
            P += dP1;
            curveDataPtr[currentVertex].position = P;
            curveDataPtr[currentVertex++].color = kOrange;
            dP1 += dP2;
            dP2 += dP3;
        }
    }
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
        mPointVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, mCount,
                                                                                         NULL, kDefaultUsage);
    }

	IvCPVertex *pointDataPtr = (IvCPVertex*) mPointVertices->BeginLoadData();
    
    for ( UInt32 i = 0; i < mCount; ++i )
    {
        pointDataPtr[i].position = mPositions[i];
        pointDataPtr[i].color = kRed;
    }
    if (!mPointVertices->EndLoadData())
        return false;

    // set up sample tangents
    // make sure the vertex buffer is appropriate for current curve data
    if ( mTangentVertices && mTangentVertices->GetVertexCount() != 4*(mCount-1) )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mTangentVertices);
        mTangentVertices = 0;
    }

    if ( mTangentVertices == 0 )
    {
        mTangentVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 4*(mCount-1),
                                                                                           NULL, kDefaultUsage);
    }

	IvCPVertex *tangentDataPtr = (IvCPVertex*) mTangentVertices->BeginLoadData();
    currentVertex = 0;

    for ( UInt32 i = 0; i < mCount-1; ++i )
    {
        tangentDataPtr[currentVertex].position = mPositions[i];
        tangentDataPtr[currentVertex++].color = kYellow;

        tangentDataPtr[currentVertex].position = mPositions[i] + 0.5f*mOutTangents[i];
        tangentDataPtr[currentVertex++].color = kYellow;

        tangentDataPtr[currentVertex].position = mPositions[i+1];
        tangentDataPtr[currentVertex++].color = kYellow;

        tangentDataPtr[currentVertex].position = mPositions[i+1] - 0.5f*mInTangents[i];
        tangentDataPtr[currentVertex++].color = kYellow;
    }
    if (!mTangentVertices->EndLoadData())
        return false;

    return true;

}   // End of IvHermite::RebuildVertexBuffers()


//-------------------------------------------------------------------------------
// @ IvHermite::Render()
//-------------------------------------------------------------------------------
// Render spline 
// Uses forward differencing
//-------------------------------------------------------------------------------
void 
IvHermite::Render()
{
    IvRenderer::mRenderer->Draw( kLineStripPrim, mCurveVertices );
    IvRenderer::mRenderer->Draw( kPointListPrim, mPointVertices );
    IvRenderer::mRenderer->Draw( kLineListPrim, mTangentVertices );

}   // End of IvHermite::Render()
