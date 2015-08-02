//===============================================================================
// @ IvLinear.cpp
// 
// Piecewise linearly interpolated spline
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvLinear.h"
#include "IvAssert.h"
#include <IvRendererHelp.h>
#include <IvRenderer.h>
#include <IvResourceManager.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvLinear::IvLinear()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvLinear::IvLinear() :
    mPositions( 0 ),
    mTimes( 0 ),
    mCount( 0 ),
    mCurveVertices( 0 ),
    mPointVertices( 0 )
{
}   // End of IvLinear::IvLinear()


//-------------------------------------------------------------------------------
// @ IvLinear::IvLinear()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvLinear::~IvLinear()
{
    Clean();
}   // End of IvLinear::~IvLinear()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvLinear& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ IvLinear::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
IvLinear::Initialize( const IvVector3* samples, const float* times,
                      unsigned int count )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !samples || !times )
        return false;

    // set up arrays
    mPositions = new IvVector3[count];
    mTimes = new float[count];
    mCount = count;

    // copy data
    for ( unsigned int i = 0; i < count; ++i )
    {
        mPositions[i] = samples[i];
        mTimes[i] = times[i];
    }

    return RebuildVertexBuffers();

}   // End of IvLinear::Initialize()


//-------------------------------------------------------------------------------
// @ IvLinear::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void 
IvLinear::Clean()
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
    delete[] mTimes;

    mCount = 0;

}   // End of IvLinear::Clean()


//-------------------------------------------------------------------------------
// @ IvLinear::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
IvVector3
IvLinear::Evaluate( float t )
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
    return (1-u)*mPositions[i] + u*mPositions[i+1];

}   // End of IvLinear::Initialize()



//-------------------------------------------------------------------------------
// @ IvLinear::RebuildVertexBuffers()
//-------------------------------------------------------------------------------
// Rebuilds vertex buffer rendering data for newly created spline
//-------------------------------------------------------------------------------
bool 
IvLinear::RebuildVertexBuffers()
{
    // build linear spline
    // make sure the vertex buffer is appropriate for current curve data
    if ( mCurveVertices && mCurveVertices->GetVertexCount() != mCount )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mCurveVertices);
        mCurveVertices = 0;
    }

    if ( mCurveVertices == 0 )
    {
        mCurveVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, mCount,
                                                                                         NULL, kDefaultUsage);
    }

    IvCPVertex* curveDataPtr = (IvCPVertex*) mCurveVertices->BeginLoadData();
    for ( UInt32 i = 0; i < mCount; ++i )
    {
        curveDataPtr[i].position = mPositions[i];
        curveDataPtr[i].color = kOrange;
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

}   // End of IvLinear::RebuildVertexBuffers()


//-------------------------------------------------------------------------------
// @ IvLinear::Render()
//-------------------------------------------------------------------------------
// Render spline
//-------------------------------------------------------------------------------
void 
IvLinear::Render()
{
    IvRenderer::mRenderer->Draw( kLineStripPrim, mCurveVertices );
    IvRenderer::mRenderer->Draw( kPointListPrim, mPointVertices );

}   // End of IvLinear::Render()
