//===============================================================================
// @ IvVertexFormats.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
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
    kCPFormat,      // color, position
    kNPFormat,      // normal, position
	kCNPFormat,		// color, normal, position
    kTCPFormat,     // texture coord, color, position
    kTNPFormat,      // texture coord, normal, position
    kVertexFormatCount
};

// individual format structs
// note that in a "real" renderer some of these would make use of compressed data

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
    sizeof(IvCPVertex),
    sizeof(IvNPVertex),
    sizeof(IvCNPVertex),
    sizeof(IvTCPVertex),
    sizeof(IvTNPVertex)
};


#endif
