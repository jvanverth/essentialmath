//===============================================================================
// @ IvTextureFormats.h
// 
// Shared format definitions for textures
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvTextureFormats__h__
#define __IvTextureFormats__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

// enum for requesting format
enum IvTextureFormat 
{
    kRGBA32TexFmt,
    kRGB24TexFmt,
    
    kLastTexFmt = kRGB24TexFmt
};
static const int kTexFmtCount = kLastTexFmt+1;

struct IvTexColorRGBA
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct IvTexColorRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};


#endif
