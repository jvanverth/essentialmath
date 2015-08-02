//===============================================================================
// @ IvReader.h
// 
// Base class for text input
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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
