//===============================================================================
// @ IvLinear.h
// 
// Piecewise linearly interpolated spline
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

#ifndef __IvLinear__h__
#define __IvLinear__h__

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

class IvLinear
{
public:
    // constructor/destructor
    IvLinear();
    ~IvLinear();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvLinear& source );

    // set up
    bool Initialize( const IvVector3* samples, const float* times,
                     unsigned int count );

    // clean up
    void Clean();

    // evaluate position
    IvVector3 Evaluate( float t );

    // render curve
    void Render();

protected:
    bool  RebuildVertexBuffers();

    IvVector3*      mPositions; // sample points
    float*          mTimes;     // time to arrive at each point
    unsigned int    mCount;     // number of points and times

    // render data
    IvVertexBuffer* mCurveVertices; // rendererable vertex data for curve
    IvVertexBuffer* mPointVertices; // rendererable vertex data for sample points
  
private:
    // copy operations
    // made private so they can't be used
    IvLinear( const IvLinear& other );
    IvLinear& operator=( const IvLinear& other );
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
