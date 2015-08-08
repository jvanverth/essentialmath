//===============================================================================
// @ IvColor.h
// 
// Base color type
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvColor__h__
#define __IvColor__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvTypes.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

struct IvColor
{
    UChar8 mRed;
    UChar8 mGreen;
    UChar8 mBlue;
    UChar8 mAlpha;
    inline void Set(UChar8 r, UChar8 g, UChar8 b, UChar8 a) { mRed = r; mGreen = g; mBlue = b; mAlpha = a; }
};

#endif
