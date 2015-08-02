//===============================================================================
// @ IvBezier.h
// 
// Piecewise Bezier spline
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvBezier__h__
#define __IvBezier__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexBuffer;
struct IvCPVertex;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvBezier
{
public:
    // constructor/destructor
    IvBezier();
    ~IvBezier();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvBezier& source );

    // set up
    bool Initialize( const IvVector3* positions, 
                     const IvVector3* controls,
                     const float* times,
                     unsigned int count );

    // automatically create control points
    bool Initialize( const IvVector3* positions, 
                     const float* times,
                     unsigned int count );

    // clean out
    void Clean();

    // evaluate position
    IvVector3 Evaluate( float t );

    // evaluate velocity
    IvVector3 Velocity( float t );

    // evaluate acceleration
    IvVector3 Acceleration( float t );

    // find parameter that moves s distance from Q(t1)
    float FindParameterByDistance( float t1, float s );

    // return length of curve between 1 and u2
    float ArcLength( float t1, float t2 );

    // get total length of curve
    inline float GetLength() { return mTotalLength; }

    // render curve
    void Render();

protected:
    float SubdivideLength( const IvVector3& P0, const IvVector3& P1, 
                    const IvVector3& P2, const IvVector3& P3 );

    // return length of curve between u1 and u2
    float SegmentArcLength( UInt32 i, float u1, float u2 );

    bool  RebuildVertexBuffers();
    unsigned int CountSubdivideVerts( const IvVector3& P0, const IvVector3& P1, 
                                const IvVector3& P2, const IvVector3& P3 );
    IvCPVertex* SubdivideCurve( IvCPVertex* currentVertex, const IvVector3& P0, const IvVector3& P1, 
                                const IvVector3& P2, const IvVector3& P3 );

    IvVector3*      mPositions;     // interpolating positions
    IvVector3*      mControls;      // approximating positions
    float*          mTimes;         // time to arrive at each point
    float*          mLengths;       // length of each curve segment
    float           mTotalLength;   // total length of curve
    unsigned int    mCount;         // number of points and times

    IvVertexBuffer* mCurveVertices; // rendererable vertex data for curve
    IvVertexBuffer* mPointVertices; // rendererable vertex data for sample points
    IvVertexBuffer* mControlVertices; // rendererable vertex data for sample points

private:
    // copy operations
    // made private so they can't be used
    IvBezier( const IvBezier& other );
    IvBezier& operator=( const IvBezier& other );
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
