//===============================================================================
// @ IvVertexFormats.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Usage notes
//===============================================================================

#ifndef __IvVertexFormats__h__
#define __IvVertexFormats__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvVector2.h"
#include "IvVector3.h"
#include "IvVector4.h"
#include "IvColor.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

// enum for requesting format
enum IvVertexFormat 
{
    kPFormat,       // position only
    kCPFormat,      // color, position
    kNPFormat,      // normal, position
    kCNPFormat,     // color, normal, position
    kTCPFormat,     // texture coord, color, position
    kTNPFormat,      // texture coord, normal, position

    kLastVertexFormat = kTNPFormat

};
const int kVertexFormatCount = kLastVertexFormat + 1;

// individual format structs
// note that in a "real" renderer some of these would make use of compressed data

// a bit silly, but consistent
struct IvPVertex
{
    IvVector3 position;
};

struct IvCPVertex
{
    IvColor color;
    IvVector3 position;
};

struct IvNPVertex
{
    IvVector3 normal;
    IvVector3 position;
};

struct IvCNPVertex
{
    IvColor color;
    IvVector3 normal;
    IvVector3 position;
};

struct IvTCPVertex
{
    IvVector2 texturecoord;
    IvColor color;
    IvVector3 position;
};

struct IvTNPVertex
{
    IvVector2 texturecoord;
    IvVector3 normal;
    IvVector3 position;
};

const unsigned int kIvVFSize[] =
{
    sizeof(IvPVertex),
    sizeof(IvCPVertex),
    sizeof(IvNPVertex),
    sizeof(IvCNPVertex),
    sizeof(IvTCPVertex),
    sizeof(IvTNPVertex)
};


#endif
