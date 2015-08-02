//===============================================================================
// @ IvMersenne.cpp
// 
// Class to compute Mersenne Twister random number generator
//
// Note: this class implements the original Mersenne Twister.  It does not use 
// the faster SIMD-oriented Mersenne Twister as that uses 64-bit integers.
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//===============================================================================

#include "IvMersenne.h"

#define USE_MT19937
#undef USE_MT11213A

#if defined(USE_MT19937)
// values for MT19937
const UInt32 kN = 624;            // array size
const UInt32 kM = 397;            // lag (for blending)
const UInt32 kR = 31;             // masking bits
const UInt32 kA = 0x9908B0DF;     // twist value
const UInt32 kU = 11;             // rest are tempering parameters
const UInt32 kS = 7;
const UInt32 kT = 15;
const UInt32 kL = 18;
const UInt32 kB = 0x9D2C5680;
const UInt32 kC = 0xEFC60000;
#elif defined( USE_MT11213A )
// values for MT11213A
// shorter period, but takes less space
const UInt32 kN = 351;
const UInt32 kM = 175;
const UInt32 kR = 19;
const UInt32 kA = 0xE4BD75F5;
const UInt32 kU = 11;
const UInt32 kS = 7;
const UInt32 kT = 15;
const UInt32 kL = 18;
const UInt32 kB = 0x655E5280;
const UInt32 kC = 0xFFD58000;
#endif

const UInt32 kLowerMask = ((UInt32)(0x00000001) << kR) - 1;   // lower R bits
const UInt32 kUpperMask = 0xffffffff << kR;  // upper (32-R) bits
const UInt32 kTwistMask = 0x00000001;
const UInt32 kMag01[2] = {0, kA};

//-------------------------------------------------------------------------------
// @ IvMersenne::IvMersenne()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
IvMersenne::IvMersenne( UInt32 seed )
{
    mStateVector = new UInt32[kN];
    // initialize the state vector
    mStateVector[0] = seed;
    for ( UInt32 i = 1; i < kN; ++i )
    {
        // randomly generate the initial state values
        // this generator ensures that both the most significant bits and least 
        // significant bits are well mixed
        mStateVector[i] = (1812433253UL * (mStateVector[i-1] ^ (mStateVector[i-1] >> 30)) + i);
    }
    mCurrentEntry = kN;
}

//-------------------------------------------------------------------------------
// @ IvMersenne::~IvMersenne()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvMersenne::~IvMersenne()
{
    delete [] mStateVector;
}

//-------------------------------------------------------------------------------
// @ IvMersenne::Random()
//-------------------------------------------------------------------------------
// Returns random integer
//-------------------------------------------------------------------------------
UInt32
IvMersenne::Random()
{
    // if we've exhausted the current state array
    if (mCurrentEntry >= kN)
    {
        // regenerate
        // do the first N-M values
        for ( UInt32 k = 0; k < kN-kM; ++k )
        {
            UInt32 temp = ((mStateVector[k]&kUpperMask) | (mStateVector[k+1]&kLowerMask));
            // new state value is: (value from M away) XOR (new blended value shifted) XOR (twist)
            mStateVector[k] = mStateVector[k + kM] ^ (temp >> 1) ^ kMag01[temp & kTwistMask];
        }
        // then all but the last value, being careful of wrapping
        for ( UInt32 k = kN-kM; k < kN-1; ++k )
        {
            UInt32 temp = ((mStateVector[k]&kUpperMask) | (mStateVector[k+1]&kLowerMask));
            mStateVector[k] = mStateVector[k + (kM-kN)] ^ (temp >> 1) ^ kMag01[temp & kTwistMask];
        }
        // then the last
        UInt32 temp = ((mStateVector[kN-1]&kUpperMask) | (mStateVector[0]&kLowerMask));
        mStateVector[kN-1] = mStateVector[kM-1] ^ (temp >> 1) ^ kMag01[temp & kTwistMask];

        mCurrentEntry = 0;
    }

    // get the next value
    UInt32 y = mStateVector[mCurrentEntry++];

    // temper it
    y ^= y >> kU;
    y ^= (y << kS) & kB;
    y ^= (y << kT) & kC;
    y ^= y >> kL;

    return y;
}


