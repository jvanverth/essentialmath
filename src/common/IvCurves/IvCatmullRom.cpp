//===============================================================================
// @ IvCatmullRom.cpp
// 
// CatmullRom spline
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

#include "IvCatmullRom.h"
#include <IvAssert.h>
#include <IvMath.h>
#include <IvRendererHelp.h>
#include <IvRenderer.h>
#include <IvResourceManager.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>
#include <float.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvCatmullRom::IvCatmullRom()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvCatmullRom::IvCatmullRom() :
    mPositions( 0 ),
    mTimes( 0 ),
    mLengths( 0 ),
    mTotalLength( 0.0f ),
    mCount( 0 ),
    mCurveVertices( 0 ),
    mPointVertices( 0 )
{
}   // End of IvCatmullRom::IvCatmullRom()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::IvCatmullRom()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvCatmullRom::~IvCatmullRom()
{
    Clean();

}   // End of IvCatmullRom::~IvCatmullRom()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvCatmullRom& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
IvCatmullRom::Initialize( const IvVector3* positions,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 4 || !positions || !times )
        return false;

    // set up arrays
    mPositions = new IvVector3[count];
    mTimes = new float[count];
    mCount = count;

    // copy data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
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

    return RebuildVertexBuffers();

}   // End of IvCatmullRom::Initialize()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void IvCatmullRom::Clean()
{
    if (mCurveVertices)
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCurveVertices);
        mCurveVertices = 0;
    }
    if (mPointVertices)
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mPointVertices);
        mPointVertices = 0;
    }

    delete[] mPositions;
    delete [] mTimes;
    delete [] mLengths;
    mTotalLength = 0.0f;
    mCount = 0;

}   // End of IvCatmullRom::Clean()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
IvVector3
IvCatmullRom::Evaluate( float t )
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
    unsigned int i;  // segment #
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t <= mTimes[i+1] )
        {
            break;
        }
    }
    ASSERT( i >= 0 && i < mCount );

    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // quadratic Catmull-Rom for Q_0
    if (i == 0)
    {
        IvVector3 A = mPositions[0] - 2.0f*mPositions[1] + mPositions[2];
        IvVector3 B = 4.0f*mPositions[1] - 3.0f*mPositions[0] - mPositions[2];
        
        return mPositions[0] + (0.5f*u)*(B + u*A);
    }
    // quadratic Catmull-Rom for Q_n-1
    else if (i >= mCount-2)
    {
        i = mCount-2;
        IvVector3 A = mPositions[i-1] - 2.0f*mPositions[i] + mPositions[i+1];
        IvVector3 B = mPositions[i+1] - mPositions[i-1];
        
        return mPositions[i] + (0.5f*u)*(B + u*A);
    }
    // cubic Catmull-Rom for interior segments
    else
    {
        // evaluate
        IvVector3 A = 3.0f*mPositions[i]
                    - mPositions[i-1]
                    - 3.0f*mPositions[i+1]
                    + mPositions[i+2];
        IvVector3 B = 2.0f*mPositions[i-1]
                    - 5.0f*mPositions[i]
                    + 4.0f*mPositions[i+1]
                    - mPositions[i+2];
        IvVector3 C = mPositions[i+1] - mPositions[i-1];
    
        return mPositions[i] + (0.5f*u)*(C + u*(B + u*A));
    }

}   // End of IvCatmullRom::Evaluate()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::Velocity()
//-------------------------------------------------------------------------------
// Evaluate derivative at parameter t
//-------------------------------------------------------------------------------
IvVector3
IvCatmullRom::Velocity( float t )
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
    // quadratic Catmull-Rom for Q_0
    if (i == 0)
    {
        IvVector3 A = mPositions[0] - 2.0f*mPositions[1] + mPositions[2];
        IvVector3 B = 4.0f*mPositions[1] - 3.0f*mPositions[0] - mPositions[2];
        
        return 0.5f*B + u*A;
    }
    // quadratic Catmull-Rom for Q_n-1
    else if (i >= mCount-2)
    {
        i = mCount-2;
        IvVector3 A = mPositions[i-1] - 2.0f*mPositions[i] + mPositions[i+1];
        IvVector3 B = mPositions[i+1] - mPositions[i-1];
        
        return 0.5f*B + u*A;
    }
    // cubic Catmull-Rom for interior segments
    else
    {
        // evaluate
        IvVector3 A = 3.0f*mPositions[i]
                    - mPositions[i-1]
                    - 3.0f*mPositions[i+1]
                    + mPositions[i+2];
        IvVector3 B = 2.0f*mPositions[i-1]
                    - 5.0f*mPositions[i]
                    + 4.0f*mPositions[i+1]
                    - mPositions[i+2];
        IvVector3 C = mPositions[i+1] - mPositions[i-1];
    
        return 0.5f*C + u*(B + 1.5f*u*A);
    }

}   // End of IvCatmullRom::Velocity()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate second derivative at parameter t
//-------------------------------------------------------------------------------
IvVector3
IvCatmullRom::Acceleration( float t )
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
    // quadratic Catmull-Rom for Q_0
    if (i == 0)
    {
        return mPositions[0] - 2.0f*mPositions[1] + mPositions[2];
    }
    // quadratic Catmull-Rom for Q_n-1
    else if (i >= mCount-2)
    {
        i = mCount-2;
        return mPositions[i-1] - 2.0f*mPositions[i] + mPositions[i+1];
    }
    // cubic Catmull-Rom for interior segments
    else
    {
        // evaluate
        IvVector3 A = 3.0f*mPositions[i]
                    - mPositions[i-1]
                    - 3.0f*mPositions[i+1]
                    + mPositions[i+2];
        IvVector3 B = 2.0f*mPositions[i-1]
                    - 5.0f*mPositions[i]
                    + 4.0f*mPositions[i+1]
                    - mPositions[i+2];
    
        return B + (3.0f*u)*A;
    }

}   // End of IvCatmullRom::Acceleration()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::FindParameterByDistance()
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
IvCatmullRom::FindParameterByDistance( float t1, float s )
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

}   // End of IvCatmullRom::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
IvCatmullRom::ArcLength( float t1, float t2 )
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
        if ( t1 <= mTimes[seg1+1] )
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

}   // End of IvCatmullRom::ArcLength()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::SegmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
IvCatmullRom::SegmentArcLength( UInt32 i, float u1, float u2 )
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
    IvVector3 A, B, C;
    if (i == 0)
    {
        A = mPositions[0] - 2.0f*mPositions[1] + mPositions[2];
        B = 4.0f*mPositions[1] - 3.0f*mPositions[0] - mPositions[2];
        
    }
    // quadratic Catmull-Rom for Q_n-1
    else if (i >= mCount-2)
    {
        i = mCount-2;
        A = mPositions[i-1] - 2.0f*mPositions[i] + mPositions[i+1];
        B = mPositions[i+1] - mPositions[i-1];
    }
    // cubic Catmull-Rom for interior segments
    else
    {
        A = 3.0f*mPositions[i]
                    - mPositions[i-1]
                    - 3.0f*mPositions[i+1]
                    + mPositions[i+2];
        B = 2.0f*mPositions[i-1]
                    - 5.0f*mPositions[i]
                    + 4.0f*mPositions[i+1]
                    - mPositions[i+2];
        C = mPositions[i+1] - mPositions[i-1];
    }
    
    for ( UInt32 j = 0; j < 5; ++j )
    {
        float u = 0.5f*((u2 - u1)*x[j] + u2 + u1);
        IvVector3 derivative;
        if ( i == 0 || i >= mCount-2)
            derivative = 0.5f*B + u*A;
        else
            derivative = 0.5f*C + u*(B + 1.5f*u*A);
        sum += c[j]*derivative.Length();
    }
    sum *= 0.5f*(u2-u1);

    return sum;

}   // End of IvCatmullRom::SegmentArcLength()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::RebuildVertexBuffers()
//-------------------------------------------------------------------------------
// Rebuilds vertex buffer rendering data for newly created spline
// Uses direct evaluation
//-------------------------------------------------------------------------------
bool 
IvCatmullRom::RebuildVertexBuffers()
{
    // build Catmull-Rom spline

    // make sure the vertex buffer is appropriate for current curve data

    // count verts
    UInt32 numverts = 1;    // for final vertex
    for (float t = mTimes[0]; t <= mTimes[mCount-1]; t += 0.1f)
    {
        ++numverts;
    }

    if ( mCurveVertices && mCurveVertices->GetVertexCount() != numverts )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCurveVertices);
        mCurveVertices = 0;
    }

    if ( mCurveVertices == 0 )
    {
        mCurveVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, numverts,
                                                                                         nullptr, kDefaultUsage);
    }

    IvCPVertex* curveDataPtr = (IvCPVertex*) mCurveVertices->BeginLoadData();

    UInt32 currentVertex = 0;
    for (float t = mTimes[0]; t <= mTimes[mCount-1]; t += 0.1f)
    {
        curveDataPtr[currentVertex].position = Evaluate( t );
        curveDataPtr[currentVertex++].color = kOrange;
    }
    curveDataPtr[currentVertex].position = Evaluate( mTimes[mCount-1] );
    curveDataPtr[currentVertex++].color = kOrange;
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
                                                                                         nullptr, kDefaultUsage);
    }

    IvCPVertex *pointDataPtr = (IvCPVertex*) mPointVertices->BeginLoadData();
    
    for ( UInt32 i = 0; i < mCount; ++i )
    {
        pointDataPtr[i].position = mPositions[i];
        pointDataPtr[i].color = kRed;
    }
    if (!mPointVertices->EndLoadData())
        return false;

    return true;

}   // End of IvCatmullRom::RebuildVertexBuffers()


//-------------------------------------------------------------------------------
// @ IvCatmullRom::Render()
//-------------------------------------------------------------------------------
// Render spline 
//-------------------------------------------------------------------------------
void 
IvCatmullRom::Render()
{
    IvRenderer::mRenderer->Draw( kLineStripPrim, mCurveVertices );
    IvRenderer::mRenderer->Draw( kPointListPrim, mPointVertices );

}   // End of IvCatmullRom::Render()
