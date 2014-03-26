//===============================================================================
// @ IvClipper.h
// ------------------------------------------------------------------------------
// Clipping class
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

#ifndef __IvClipperDefs__
#define __IvClipperDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvVector3.h>
#include <IvPlane.h>

class IvVertexBuffer;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvClipper
{
public:
    IvClipper();
    ~IvClipper();

    void ClipVertex( const IvVector3& end );
    void StartClip();
    inline void SetPlane( const IvPlane& plane ) { mPlane = plane; }
    void FinishClip();

private:
    IvPlane mPlane;                 // current clipping plane
    IvVector3 mStart;               // current edge start vertex
    float mBCStart;                 // current edge start boundary condition
    bool mStartInside;              // whether current start vertex is inside
    bool mFirstVertex;              // whether expected vertex is start vertex
    IvVector3 mClipVertices[6];    // set of currently clipped vertices
    unsigned int mNumVertices;      // number of clipped vertices
    IvVertexBuffer* mBuffer;        // output vertices
};

#endif
