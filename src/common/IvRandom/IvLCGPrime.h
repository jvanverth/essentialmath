//===============================================================================
// @ IvLCGPrime.h
// 
// Linear congruential class using prime modulus
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This RNG uses, as a default, a prime modulus and does the math necessary to
// avoid overflow with 32-bit integers.
// 
//===============================================================================

#pragma once

#include "IvTypes.h"

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvLCGPrime
{
public:
    IvLCGPrime( UInt32 seed, UInt32 m = 4294967291, UInt32 a = 93167);

    UInt32 Random();
    float  RandomFloat();
    inline UInt32 MaxRand() { return mM - 1; }

private:
    UInt32 mSeed;
    UInt32 mM;
    UInt32 mA;
    UInt32 mQ;
    UInt32 mR;
};

//-------------------------------------------------------------------------------
//-- Inline Functions -----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvLCGPrime::IvLCGPrime()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
inline
IvLCGPrime::IvLCGPrime( UInt32 seed, UInt32 m, UInt32 a )
{
    mSeed = seed;
    mM = m;
    mA = a;
    mQ = m/a;
    mR = m%a;
}

//-------------------------------------------------------------------------------
// @ IvLCG64::Random()
//-------------------------------------------------------------------------------
// Return random integer from 0 to M
//-------------------------------------------------------------------------------
inline UInt32
IvLCGPrime::Random()
{
    UInt32 mod = mA*(mSeed % mQ);
    UInt32 div = mR*(mSeed / mQ);
    if ( mod > div )
    {
        mSeed = mod-div;
    }
    else
    {
        mSeed = mM-div+mod;
    }
    return mSeed;
}


//-------------------------------------------------------------------------------
// @ IvLCG64::RandomFloat()
//-------------------------------------------------------------------------------
// Return random floating point number in range [0,1)
//
// Note that we can't use the floating point trick we use for the other RNGs
// as we don't generate random bits for the full range of an integer
//-------------------------------------------------------------------------------
inline float  
IvLCGPrime::RandomFloat()
{
    return (float)(Random())/MaxRand();
}
