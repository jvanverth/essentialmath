//===============================================================================
// @ IvKISS.h
//
// Marsaglia's Keep It Simple, Stupid generator, modified for 32-bit processors.
// **** ? created this?
// ------------------------------------------------------------------------------
// Copyright (C) 2014 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This RNG avoids issues with overflow by using an add-with-carry method.
//===============================================================================

#pragma once

#include "IvTypes.h"
#include "IvAssert.h"

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvKISS
{
public:
    IvKISS( UInt32 seed );
    
    UInt32 Random();
    float  RandomFloat();
    inline UInt32 MaxRand() { return UInt32(0xffffffff); }
    
private:
    UInt32 mX;
    UInt32 mY;
    UInt32 mZ;
    UInt32 mW;
    UInt32 mC;
};

//-------------------------------------------------------------------------------
//-- Inline Functions -----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Xorshift32
//-------------------------------------------------------------------------------
// Helper function for initialization
//-------------------------------------------------------------------------------
inline UInt32 Xorshift32(UInt32 x) {
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    
    return x;
}

//-------------------------------------------------------------------------------
// @ IvKISS::IvKISS()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
inline
IvKISS::IvKISS( UInt32 seed )
{
    ASSERT(0 != seed);
    // use 32-bit xorshift to set initial state
    mX = seed;
    mY = Xorshift32(mX);
    mZ = Xorshift32(mY);
    mW = Xorshift32(mZ);
    mC = 0;
}

//-------------------------------------------------------------------------------
// @ IvKISS::Random()
//-------------------------------------------------------------------------------
// Return random integer
//-------------------------------------------------------------------------------
inline UInt32
IvKISS::Random()
{
    // xorshift
    mY ^= (mY<<5);
    mY ^= (mY>>7);
    mY ^= (mY<<22);
    
    // add-with-carry
    Int32 t;
    t = mZ + mW + mC;
    mZ = mW;
    mC = (t < 0);
    mW = t & 2147483647;
    
    // LCG
    mX += 1411392427;  // implied mod 32
    
    return mX + mY + mW;
}

//-------------------------------------------------------------------------------
// @ IvKISS::RandomFloat()
//-------------------------------------------------------------------------------
// Return random floating point number in range [0,1)
//-------------------------------------------------------------------------------
inline float
IvKISS::RandomFloat()
{
    union
    {
        unsigned int mI;
        float        mF;
    } floatConv;
    floatConv.mI = 0x3f800000 | (Random() >> 9);
    float f = floatConv.mF - 1.0f;
    return f;
}

