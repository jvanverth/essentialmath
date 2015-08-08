//===============================================================================
// @ IvUniformBSpline.cpp
// 
// UniformBSpline spline
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
// Implementation notes:
//     A change here from the other curves.  Because the B-spline is an 
// approximating curve, it doesn't make sense to set times at each position,
// since the curve won't pass through them.  Instead, we triplicate the first 
// and last positions to force the curve to pass through them, and set times at 
// those locations.  The time interval will be subdivided equally to create
// the time at the join, or knot location, between each curve segment.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvUniformBSpline.h"
#include "IvBezier.h"
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
// @ IvUniformBSpline::IvUniformBSpline()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvUniformBSpline::IvUniformBSpline() :
    mPositions( 0 ),
    mTimes( 0 ),
    mCount( 0 ),
    mCurveVertices( 0 ),
    mPointVertices( 0 )
{
}   // End of IvUniformBSpline::IvUniformBSpline()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::IvUniformBSpline()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvUniformBSpline::~IvUniformBSpline()
{
    Clean();

}   // End of IvUniformBSpline::~IvUniformBSpline()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvUniformBSpline& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
IvUniformBSpline::Initialize( const IvVector3* positions,
                              unsigned int count,
                              float startTime, float endTime )

{
    // make sure not already initialized
    if ( mCount != 0 )
        return false;

    // make sure data is valid
    if ( count < 2 || !positions )
        return false;

    // set up arrays
    mPositions = new IvVector3[count+4];
    mTimes = new float[count+2];
    mCount = count+4;

    // copy position data
    // triplicate start and end points so that curve passes through them
    mPositions[0] = mPositions[1] = positions[0];
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i+2] = positions[i];
    }
    mPositions[mCount-1] = mPositions[mCount-2] = positions[count-1];

    // now set up times
    // we subdivide interval to get arrival times at each knot location
    float dt = (endTime - startTime)/(float)(count+1);
    mTimes[0] = startTime;
    for ( i = 0; i < count; ++i )
    {
        mTimes[i+1] = mTimes[i]+dt;
    }
    mTimes[count+1] = endTime;

    // set up curve segment lengths
    mLengths = new float[mCount-3];
    mTotalLength = 0.0f;
    for ( i = 0; i < mCount-3; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return RebuildVertexBuffers();

}   // End of IvUniformBSpline::Initialize()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void
IvUniformBSpline::Clean()
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

}   // End of IvUniformBSpline::~Clean()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
IvVector3
IvUniformBSpline::Evaluate( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 6 );
    if ( mCount < 6 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mPositions[0];
    else if ( t >= mTimes[mCount-3] )
        return mPositions[mCount-3];

    // find segment and parameter
    unsigned int i;  // segment #
    for ( i = 0; i < mCount-3; ++i )
    {
        if ( t <= mTimes[i+1] )
        {
            break;
        }
    }
    ASSERT( i >= 0 && i < mCount-3 );

    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // match segment index to standard B-spline terminology
    i += 3;

    // evaluate
    IvVector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    IvVector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    IvVector3 C = 3.0f*mPositions[i-1] - 3.0f*mPositions[i-3];
    IvVector3 D = mPositions[i-1]
                + 4.0f*mPositions[i-2]
                + mPositions[i-3];

    return (D + u*(C + u*(B + u*A)))/6.0f;

}   // End of IvUniformBSpline::Evaluate()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::Velocity()
//-------------------------------------------------------------------------------
// Evaluate derivative at parameter t
//-------------------------------------------------------------------------------
IvVector3
IvUniformBSpline::Velocity( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        t = mTimes[0];
    else if ( t >= mTimes[mCount-3] )
        t = mTimes[mCount-3];

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

    // match segment index to standard B-spline terminology
    i += 3;

    // evaluate
    IvVector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    IvVector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    IvVector3 C = 3.0f*mPositions[i-1] - 3.0f*mPositions[i-3];
    
    IvVector3 result = (C + u*(2.0f*B + 3.0f*u*A))/6.0f;

    return result;

}   // End of IvUniformBSpline::Velocity()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate second derivative at parameter t
//-------------------------------------------------------------------------------
IvVector3
IvUniformBSpline::Acceleration( float t )
{
    // make sure data is valid
    ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return IvVector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        t = 0.0f;
    else if ( t > mTimes[mCount-3] )
        t = mTimes[mCount-3];

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

    // match segment index to standard B-spline terminology
    i += 3;

    // evaluate
    IvVector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    IvVector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    
    return 1.0f/3.0f*B + u*A;

}   // End of IvUniformBSpline::Acceleration()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::FindParameterByDistance()
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
IvUniformBSpline::FindParameterByDistance( float t1, float s )
{
    // initialize bisection endpoints
    float a = t1;
    float b = mTimes[mCount-3];

    // ensure that we remain within valid parameter space
    if ( s >= ArcLength(t1, b) )
        return b;
    if ( s <= 0.0f )
        return a;

    // make first guess
    float p = t1 + s*(mTimes[mCount-3]-mTimes[0])/mTotalLength;

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

}   // End of IvUniformBSpline::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
IvUniformBSpline::ArcLength( float t1, float t2 )
{
    if ( t2 <= t1 )
        return 0.0f;

    if ( t1 < mTimes[0] )
        t1 = mTimes[0];

    if ( t2 > mTimes[mCount-3] )
        t2 = mTimes[mCount-3];

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

}   // End of IvUniformBSpline::ArcLength()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::SegmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
IvUniformBSpline::SegmentArcLength( UInt32 i, float u1, float u2 )
{
    static const float x[] =
    {
        0.0000000000f, 0.5384693101f, -0.5384693101f, 0.9061798459f, -0.9061798459f 
    };

    static const float c[] =
    {
        0.5688888889f, 0.4786286705f, 0.4786286705f, 0.2369268850f, 0.2369268850f
    };

    ASSERT(i >= 0 && i < mCount-3);

    if ( u2 <= u1 )
        return 0.0f;

    if ( u1 < 0.0f )
        u1 = 0.0f;

    if ( u2 > 1.0f )
        u2 = 1.0f;

    // reindex to use standard B-spline segment count
    i += 3;

    // use Gaussian quadrature
    float sum = 0.0f;
    // set up for computation of IvUniformBSpline derivative
    IvVector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    IvVector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    IvVector3 C = 3.0f*mPositions[i-1] - 3.0f*mPositions[i-3];
    
    for ( UInt32 j = 0; j < 5; ++j )
    {
        float u = 0.5f*((u2 - u1)*x[j] + u2 + u1);
        IvVector3 derivative = (C + u*(2.0f*B + 3.0f*u*A))/6.0f;
        sum += c[j]*derivative.Length();
    }
    sum *= 0.5f*(u2-u1);

    return sum;

}   // End of IvUniformBSpline::SegmentArcLength()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::RebuildVertexBuffers()
//-------------------------------------------------------------------------------
// Rebuilds vertex buffer rendering data for newly created spline
// Two interesting things here:
//     First is the conversion from uniform B-spline to Bezier curve
//     Second is the use of forward differencing 
//-------------------------------------------------------------------------------
bool 
IvUniformBSpline::RebuildVertexBuffers()
{
    const UInt32 vertsPerSegment = 10;
    // build B-spline

    // make sure the vertex buffer is appropriate for current curve data
    if ( mCurveVertices && mCurveVertices->GetVertexCount() != vertsPerSegment*(mCount-3) )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCurveVertices);
        mCurveVertices = 0;
    }

    if ( mCurveVertices == 0 )
    {
        mCurveVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat,
                                                                                         vertsPerSegment*(mCount-3),
                                                                                         NULL, kDefaultUsage);
    }

    IvCPVertex* curveDataPtr = (IvCPVertex*) mCurveVertices->BeginLoadData();
    UInt32 currentVertex = 0;

    // set up first control points
    IvVector3 controls[4];
    IvVector3 l = (2.0f*mPositions[1] + mPositions[0])/3.0f;
    controls[1] = (2.0f*mPositions[1] + mPositions[2])/3.0f;
    controls[0] = 0.5f*(l + controls[1]);

    for ( UInt32 i = 3; i < mCount; ++i )
    {
        // build control points
        IvVector3 r = (2.0f*mPositions[i-1] + mPositions[i])/3.0f;
                
        controls[2] = (2.0f*mPositions[i-1] + mPositions[i-2])/3.0f;
        controls[3] = 0.5f*(r + controls[2]);

        // set up step variables for forward differencing
        const float h = 1.0f/(float)(vertsPerSegment-1);
        const float h2 = h*h;
        const float h3 = h2*h;
        IvVector3 A = controls[3]
                    - 3.0f*controls[2]
                    + 3.0f*controls[1]
                    - controls[0];
        IvVector3 B = 3.0f*controls[2]
                    - 6.0f*controls[1]
                    + 3.0f*controls[0];
        IvVector3 C = 3.0f*controls[1]
                    - 3.0f*controls[0];
        IvVector3 dP1 = h3*A + h2*B + h*C;
        IvVector3 dP3 = 6.0f*h3*A;
        IvVector3 dP2 = dP3 + 2.0f*h2*B;

        // output first point
        IvVector3 P = controls[0];
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

        // copy for next segment
        l = controls[2];
        controls[0] = controls[3];
        controls[1] = r;
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

    return true;

}   // End of IvUniformBSpline::RebuildVertexBuffers()


//-------------------------------------------------------------------------------
// @ IvUniformBSpline::Render()
//-------------------------------------------------------------------------------
// Render spline
// Two interesting things here:
//     First is the conversion from uniform B-spline to Bezier curve
//     Second is the use of OpenGL to draw the Bezier curve
//-------------------------------------------------------------------------------
void 
IvUniformBSpline::Render()
{
    IvRenderer::mRenderer->Draw( kLineStripPrim, mCurveVertices );
    IvRenderer::mRenderer->Draw( kPointListPrim, mPointVertices );

/*    const int subdivisions = 10;

    // draw UniformBSpline spline
    glLineWidth(2.0);
    glColor3f(1.0f, 0.5f, 0.0f);
    GLboolean lighting = false;
    glGetBooleanv(GL_LIGHTING, &lighting);
    if ( lighting )
        glDisable(GL_LIGHTING);

    UInt32 i;
    glMapGrid1f(subdivisions, 0.0f, 1.0f);

    glEnable( GL_MAP1_VERTEX_3 );

    // set up first control points
    IvVector3 controls[4];
    IvVector3 l = (2.0f*mPositions[1] + mPositions[0])/3.0f;
    controls[1] = (2.0f*mPositions[1] + mPositions[2])/3.0f;
    controls[0] = 0.5f*(l + controls[1]);

    for ( i = 3; i < mCount; ++i )
    {
        // build control points
        IvVector3 r = (2.0f*mPositions[i-1] + mPositions[i])/3.0f;
                
        controls[2] = (2.0f*mPositions[i-1] + mPositions[i-2])/3.0f;
        controls[3] = 0.5f*(r + controls[2]);

        glMap1f( GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, 4,
                    (float*) &controls[0].x );
        // to see each curve segment, use subdivisions-1 here
        glEvalMesh1(GL_LINE, 0, subdivisions);

        // copy for next segment
        l = controls[2];
        controls[0] = controls[3];
        controls[1] = r;
    }

    // draw approximating points
    glPointSize( 5.0f );
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 0.0f);
    for ( i = 0; i < mCount; ++i )
    {
        glVertex3f(mPositions[i].x, mPositions[i].y, mPositions[i].z);        
    }
    glEnd();

    if (lighting)
        glEnable(GL_LIGHTING);
*/
}   // End of IvUniformBSpline::Render()
