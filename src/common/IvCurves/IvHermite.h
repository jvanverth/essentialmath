//===============================================================================
// @ IvHermite.h
// 
// Hermite spline
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
//===============================================================================

#ifndef __IvHermite__h__
#define __IvHermite__h__

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

class IvHermite
{
public:
    // constructor/destructor
    IvHermite();
    ~IvHermite();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvHermite& source );

    // set up
    // default
    bool Initialize( const IvVector3* positions, 
                     const IvVector3* inTangents,
                     const IvVector3* outTangents,
                     const float* times,
                     unsigned int count );
    // clamped spline
    bool InitializeClamped( const IvVector3* positions, 
                            const float* times,
                            unsigned int count,
                            const IvVector3& inTangent,
                            const IvVector3& outTangent );
    // natural spline
    bool InitializeNatural( const IvVector3* positions, 
                            const float* times,
                            unsigned int count );

    // destroy
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

    IvVector3*      mPositions;     // sample positions
    IvVector3*      mInTangents;    // incoming tangents on each segment
    IvVector3*      mOutTangents;   // outgoing tangents on each segment
    float*          mTimes;         // time to arrive at each point
    float*          mLengths;       // length of each curve segment
    float           mTotalLength;   // total length of curve
    unsigned int    mCount;         // number of points and times

    // render data
    IvVertexBuffer* mCurveVertices; // rendererable vertex data for curve
    IvVertexBuffer* mPointVertices; // rendererable vertex data for sample points
    IvVertexBuffer* mTangentVertices; // rendererable vertex data for sample tangents
  
private:
    // copy operations
    // made private so they can't be used
    IvHermite( const IvHermite& other );
    IvHermite& operator=( const IvHermite& other );
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
