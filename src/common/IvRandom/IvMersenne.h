//===============================================================================
// @ IvMersenne.h
// 
// Mersenne Twister routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// Class to compute Mersenne Twister random number generator
//
// This class uses the Mersenne Twister type MT19937.  It does not use the faster
// SIMD-oriented Mersenne Twister as that requires 64-bit integers.
//===============================================================================

#pragma once

#include "IvTypes.h"

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvMersenne
{
public:
    IvMersenne( UInt32 seed );
    ~IvMersenne();

    UInt32 Random();
    float  RandomFloat();
    inline UInt32 MaxRand() { return (UInt32)0xffffffff; }

private:
    UInt32* mStateVector;
    UInt32 mCurrentEntry;
};

//-------------------------------------------------------------------------------
//-- Inline Functions -----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvLCG64::RandomFloat()
//-------------------------------------------------------------------------------
// Return random floating point number in range [0,1)
//-------------------------------------------------------------------------------
inline float
IvMersenne::RandomFloat()
{
    unsigned int floatInt = 0x3f800000 | (Random() >> 9); 
    float f = *(float*)(&floatInt) - 1.0f; 
    return f;
}
