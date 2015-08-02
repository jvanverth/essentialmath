//===============================================================================
// @ IvCarryMultiply.h
// 
// Class to compute carry-multiply random number generator
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// Using George Marsaglia's original proposal, as we can't always assume we have
// a 64-bit integer available.
//===============================================================================

#pragma once

#include "IvTypes.h"
#include "IvLCGPrime.h"

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvCarryMultiply
{
public:
    IvCarryMultiply( UInt32 seed );

    UInt32 Random();
    float  RandomFloat();
    inline UInt32 MaxRand() { return UInt32(0xffffffff); }

private:
    UInt32 mK;
    UInt32 mJ;
    UInt32 mI;
    UInt32 mL;
    UInt32 mM;
    UInt32 mN;
};

//-------------------------------------------------------------------------------
//-- Inline Functions -----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvCarryMultiply::IvCarryMultiply()
//-------------------------------------------------------------------------------
// Constructor
//
// Set up table of six values from seed
//-------------------------------------------------------------------------------
inline
IvCarryMultiply::IvCarryMultiply( UInt32 seed )
{
    IvLCGPrime startRNG( seed );
    mK = startRNG.Random();
    mJ = startRNG.Random();
    mI = startRNG.Random();
    mL = startRNG.Random();
    mM = startRNG.Random();
    mN = startRNG.Random();
}

//-------------------------------------------------------------------------------
// @ IvCarryMultiply::Random()
//-------------------------------------------------------------------------------
// Return random integer
//
// Updates table and generates new random integer
//-------------------------------------------------------------------------------
inline UInt32
IvCarryMultiply::Random()
{
    mK = 30903*(mK & 65535) + (mK >> 16);
    mJ = 18000*(mJ & 65535) + (mJ >> 16);
    mI = 29013*(mI & 65535) + (mI >> 16);
    mL = 30345*(mL & 65535) + (mL >> 16);
    mM = 30903*(mM & 65535) + (mM >> 16);
    mN = 31803*(mN & 65535) + (mN >> 16);
    return ((mK + mI + mM) << 16) + mJ + mL + mN;
}

//-------------------------------------------------------------------------------
// @ IvCarryMultiply::RandomFloat()
//-------------------------------------------------------------------------------
// Return random floating point number in range [0,1)
//-------------------------------------------------------------------------------
inline float  
IvCarryMultiply::RandomFloat()
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
