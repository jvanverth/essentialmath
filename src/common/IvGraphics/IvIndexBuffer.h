//===============================================================================
// @ IvIndexBuffer.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Usage notes
//===============================================================================

#ifndef __IvIndexBuffer__h__
#define __IvIndexBuffer__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvIndexBuffer
{
public:
    // interface routines
    virtual void* BeginLoadData() = 0;
    virtual bool  EndLoadData() = 0;
    
    // For now, all index buffers are unsigned int (32 bit)

    inline unsigned int GetNumIndices() { return mNumIndices; }

protected:
    // constructor/destructor
    IvIndexBuffer() : mNumIndices(0) {}
    virtual ~IvIndexBuffer() {}

    unsigned int        mNumIndices;

private:
    // copy operations (unimplemented so we can't copy)
    IvIndexBuffer(const IvIndexBuffer& other);
    IvIndexBuffer& operator=(const IvIndexBuffer& other);
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
