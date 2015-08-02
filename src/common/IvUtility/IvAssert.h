//===============================================================================
// @ IvAssert.h
// 
// Assertion macros
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#ifndef __IvAssert__h__
#define __IvAssert__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <assert.h>
#include "IvDebugger.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

#ifndef ASSERT

#ifndef NDEBUG
#define ASSERT(x) \
    if (!(x)) \
    DEBUG_OUT( "Assertion failed: file " << __FILE__ << " line " << __LINE__ << std::endl ); \
    assert(x)
#else
#define ASSERT(x)
#endif

#endif

#endif
