//===============================================================================
// @ IvClock.cpp
// 
// Time management class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#ifndef __IvClock__h__
#define __IvClock__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvWriter.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvClock
{
public:
    // constructor/destructor
    IvClock();
    ~IvClock();
    
    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvClock& source );

    // update counters
    void    Update();
    
    // get time after update
    inline float    GetCurrentTime()    { return mCurrentTime; }
    inline float    GetElapsedTime()    { return mElapsedTime; }
    
protected:
    float   mCurrentTime;
    float   mElapsedTime;

private:
    // copy operations
    IvClock(const IvClock& other);
    IvClock& operator=(const IvClock& other);
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

extern unsigned int GetTime();

#endif
