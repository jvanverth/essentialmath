//===============================================================================
// @ IvTextureFormats.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
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
