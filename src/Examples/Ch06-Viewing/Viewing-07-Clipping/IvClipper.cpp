//===============================================================================
// @ IvClipper.cpp
// ------------------------------------------------------------------------------
// Clipper object
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// This class implements a very simple clipping routine.  To use it, you set the 
// clipping plane, using SetPlane().  Then for each triangle you first call 
// StartClip().  You then call ClipVertex() for each vertex, duplicating the 
// first vertex.  Then call FinishClip() to finish things off.  The result will 
// be a clipped, rendered triangle.
//
// This is by no means production level -- just a demo.  For example, duplicating
// the first vertex could be handled by storing it and passing it in again as a 
// call to ClipVertex() in FinishClip().  In this case, we're making it explicit.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvClipper.h"
#include "IvRenderer.h"
#include "IvRendererHelp.h"
#include "IvResourceManager.h"
#include "IvVertexBuffer.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvClipper::IvClipper()
//-------------------------------------------------------------------------------
// Call before starting to clip a triangle.
// Note that the clipped triangle will be a convex polygon, so we can
// render this using a triangle fan.
//-------------------------------------------------------------------------------
IvClipper::IvClipper()
{ 
    mBuffer = 0;
    mNumVertices = 0;
    mFirstVertex = true;

}   // End of IvClipper::IvClipper()


//-------------------------------------------------------------------------------
// @ IvClipper::~IvClipper()
//-------------------------------------------------------------------------------
// Call before starting to clip a triangle.
// Note that the clipped triangle will be a convex polygon, so we can
// render this using a triangle fan.
//-------------------------------------------------------------------------------
IvClipper::~IvClipper()
{ 
    if ( mBuffer )
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(mBuffer);
        mBuffer = 0;
    }

}   // End of IvClipper::IvClipper()


//-------------------------------------------------------------------------------
// @ IvClipper::StartClip()
//-------------------------------------------------------------------------------
// Call before starting to clip a triangle.
// Note that the clipped triangle will be a convex polygon, so we can
// render this using a triangle fan.
//-------------------------------------------------------------------------------
void 
IvClipper::StartClip()
{ 
    mNumVertices = 0;
    mFirstVertex = true;

}   // End of IvClipper::StartClip()


//-------------------------------------------------------------------------------
// @ IvClipper::ClipVertex()
//-------------------------------------------------------------------------------
// Pass in next vertex
//-------------------------------------------------------------------------------
void 
IvClipper::ClipVertex( const IvVector3& end )
{
    float BCend = mPlane.Test(end);
    bool endInside = ( BCend >= 0.0f );

    if (!mFirstVertex)
    {
        // if one of the points is inside
        if ( mStartInside || endInside )
        {
            // if the start is inside, just output it
            if (mStartInside)
                mClipVertices[mNumVertices++] = mStart;

            // if one of them is outside, output clip point
            if ( !(mStartInside && endInside) )
            {
                float t;
                IvVector3 output;
                if (endInside)
                {
                    t = BCend/(BCend - mBCStart);
                    output =  end - t*(end - mStart);
                }
                else
                {
                    t = mBCStart/(mBCStart - BCend);
                    output = mStart + t*(end - mStart);
                }
                mClipVertices[mNumVertices++] = output;
            }
        }
    }
    mStart = end;
    mBCStart = BCend;
    mStartInside = endInside;
    mFirstVertex = false;

}   // End of IvClipper::ClipVertex()


//-------------------------------------------------------------------------------
// @ IvClipper::FinishClip()
//-------------------------------------------------------------------------------
// Call when done clipping triangle
//-------------------------------------------------------------------------------
void 
IvClipper::FinishClip()
{
    if ( !mBuffer )
    {
        mBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 6,
                                                                                  NULL, kDefaultUsage);
    }

    IvCPVertex* dataPtr = (IvCPVertex*) mBuffer->BeginLoadData();
    
    // stripify the convex polygon
    for (unsigned int i = 0; i < mNumVertices; ++i)
    {
        dataPtr[i].color = kRed;
        if (i == 0)
        {
            dataPtr[i].position = mClipVertices[0];
        }
        // odd vertices
        else if (i & 0x1)
        {
            dataPtr[i].position = mClipVertices[i/2+1];
        }
        // even vertices
        else
        {
            dataPtr[i].position = mClipVertices[mNumVertices - (i/2)];
        }
    }
    mBuffer->EndLoadData();

    IvRenderer::mRenderer->Draw(kTriangleStripPrim, mBuffer, mNumVertices);
        
}  // End of IvClipper::FinishClip()
