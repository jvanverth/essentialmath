//===============================================================================
// @ IvReader.h
// 
// Base class for text input
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
// 
// Defining a text stream in this way, rather than using istream directly,
// allows for two things:
// a) it can allow the implementer to force the derived stream to be text-only
//    (see IvFileReader)
// b) you can override the stream with another, less fatty implementation if
//    needed
//===============================================================================

#ifndef __IvReader__h__
#define __IvReader__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <istream>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

typedef std::istream IvReader;

#endif
