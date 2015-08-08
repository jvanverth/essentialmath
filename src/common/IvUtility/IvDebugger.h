//===============================================================================
// @ IvDebugger.h
// 
// Convenience class to dump debug information
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
// Use DEBUG_OUT combined with operator<< to dump debug text
//===============================================================================

#ifndef __IvDebugger__h__
#define __IvDebugger__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <sstream>
#include <fstream>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

#define ERROR_OUT( x ) \
{ \
    gDebugger << x; \
    gDebugger.Flush(); \
}

#ifndef NDEBUG
#define DEBUG_OUT( x )  ERROR_OUT( x )
#else
#define DEBUG_OUT( x )
#endif

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvDebugger : public std::ostringstream
{
public:
    // constructor/destructor
    IvDebugger();
    virtual ~IvDebugger();
    
    // Default (nullptr or empty filename) will turn off file logging
    void DumpToFile( const char* filename = 0 );
    
    void Flush( void );
    
protected:
    std::ofstream mOutFile;
    
private:
    // copy operations
    IvDebugger( const IvDebugger& other );
    IvDebugger& operator=( const IvDebugger& other );
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

extern IvDebugger gDebugger;

#endif
