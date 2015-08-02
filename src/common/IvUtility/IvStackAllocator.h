//===============================================================================
// @ IvStackAllocator.h
// 
// Assertion macros
// ------------------------------------------------------------------------------
// Copyright (C) 2014 by James M. Van Verth. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#ifndef __IvStackAllocator__h__
#define __IvStackAllocator__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvStackAllocator
{
public:
    IvStackAllocator(size_t totalBytes);
    ~IvStackAllocator();

    size_t GetCurrentOffset() { return fCurrentOffset; }
    void Reset(size_t offset = 0) { fCurrentOffset = offset; }

    void* Allocate(size_t bytes);

    static IvStackAllocator* mScratchAllocator;

private:
    IvStackAllocator();
    unsigned char* fAllocation;
    size_t         fTotalBytes;
    size_t         fCurrentOffset;
};


#endif
