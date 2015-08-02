//===============================================================================
// @ IvUniformBSpline.h
// 
// Uniform B-spline class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
// Usage notes:
//     A change here from the other curves.  Because the B-spline is an 
// approximating curve, it doesn't make sense to set times at each position,
// since the curve won't pass through them.  Instead, we triplicate the first 
// and last positions to force the curve to pass through them, and set times at 
// those locations.  The time interval will be subdivided equally to create
// the time at each knot location.
//===============================================================================

#ifndef __IvUniformBSpline__h__
#define __IvUniformBSpline__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBuffer;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvUniformBSpline
{
public:
    // constructor/destructor
    IvUniformBSpline();
    ~IvUniformBSpline();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvUniformBSpline& source );

    // set up
    bool Initialize( const IvVector3* positions, 
                     unsigned int count,
                     float startTime, float endTime );

    // remove data
    void Clean();

    // evaluate position
    IvVector3 Evaluate( float t );

    // evaluate derivative
    IvVector3 Velocity( float t );

    // evaluate second derivative
    IvVector3 Acceleration( float t );

    // find parameter that moves s distance from Q(t1)
    float FindParameterByDistance( float t1, float s );

    // return length of curve between t1 and t2
    float ArcLength( float t1, float t2 );

    // get total length of curve
    inline float GetLength() { return mTotalLength; }

    // render curve
    void Render();

protected:
    // return length of curve between u1 and u2
    float SegmentArcLength( UInt32 i, float u1, float u2 );

    bool  RebuildVertexBuffers();

    IvVector3*      mPositions;     // positions of control points
    float*          mTimes;         // time to approach each point
    float*          mLengths;       // length of each curve segment
    float           mTotalLength;   // total length of curve
    unsigned int    mCount;         // number of points and times

    IvVertexBuffer* mCurveVertices; // rendererable vertex data for curve
    IvVertexBuffer* mPointVertices; // rendererable vertex data for sample points

private:
    // copy operations
    // made private so they can't be used
    IvUniformBSpline( const IvUniformBSpline& other );
    IvUniformBSpline& operator=( const IvUniformBSpline& other );
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
