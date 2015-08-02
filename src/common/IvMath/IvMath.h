//===============================================================================
// @ IvMath.h
// 
// Base math macros and functions
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#ifndef __IvMath__h__
#define __IvMath__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvTypes.h"
#include <math.h>
#include <assert.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

#define kEpsilon    1.0e-6f

#define kPI         3.1415926535897932384626433832795f
#define kHalfPI     1.5707963267948966192313216916398f
#define kTwoPI      2.0f*kPI

#if defined(__APPLE__) && defined(__MACH__)
#define sqrtf sqrt
#define sinf sin
#define cosf cos
#define fabsf fabs
#define tanf tan
#define atan2f atan2
#define acosf acos
#endif

#undef IV_APPROXIMATION   // commented out for the moment
typedef union {float f; int i;} IntOrFloat;

//
// This routine is from Lomont, "Floating Point Tricks," _Game Gems 6_
// See also www.lomont.org
//
inline float IvSqrt( float val )
{ 
#if defined(IV_APPROXIMATION)
      assert(val >= 0);
      IntOrFloat workval;
      workval.f = val;
      workval.i -= 0x3f800000; // subtract 127 from biased exponent
      workval.i >>= 1;         // requires signed shift to preserve sign
      workval.i += 0x3f800000; // rebias the new exponent
      workval.i &= 0x7FFFFFFF; // remove sign bit
      return workval.f;
#else
      return sqrtf( val ); 
#endif
}

//
// This routine is a classic approximation to rsqrt, attributed to
// Greg Walsh.
// The optimized values used are from http://rrrola.wz.cz/inv_sqrt.html
//
inline float IvRecipSqrt( float val )
{ 
#if defined(IV_APPROXIMATION)
    IntOrFloat workval;
    workval.f = val;
    // initial guess y0 with magic number
    workval.i = 0x5F1FFFF9 - (workval.i>>1);
    // Newton step, increases accuracy
    const float estimate_sq = workval.f*workval.f;
    workval.f *= 0.703952253f*(2.38924456f-val*estimate_sq);
    return workval.f;
#else
    return 1.0f/sqrtf( val );
#endif
} // RecipSqrt

inline float IvAbs( float f )           { return fabsf(f); }

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Functions --------------------------------------------------------------------
//-------------------------------------------------------------------------------
extern void IvFastSinCos( float a, float& sina, float& cosa );

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvIsZero()
//-------------------------------------------------------------------------------
// Is this floating point value close to zero?
//-------------------------------------------------------------------------------
inline bool IvIsZero( float a, float epsilon = kEpsilon ) 
{
    return (IvAbs(a) <= epsilon);

}   // End of IvIsZero()

//-------------------------------------------------------------------------------
// @ IvAreEqual()
//-------------------------------------------------------------------------------
// Are these floating point values close to equal?
//-------------------------------------------------------------------------------
inline bool IvAreEqual( float a, float b, float epsilon = kEpsilon ) 
{
    return (IvAbs(a-b) <= epsilon*(IvAbs(a) + IvAbs(b) + 1.0f));

}   // End of IvAreEqual()


//-------------------------------------------------------------------------------
// @ IvSin()
//-------------------------------------------------------------------------------
// Returns the floating-point sine of the argument
//-------------------------------------------------------------------------------
inline float IvSin( float a )
{
    return sinf(a);

}  // End of IvSin


//-------------------------------------------------------------------------------
// @ IvCos()
//-------------------------------------------------------------------------------
// Returns the floating-point cosine of the argument
//-------------------------------------------------------------------------------
inline float IvCos( float a )
{
    return cosf(a);

}  // End of IvCos


//-------------------------------------------------------------------------------
// @ IvTan()
//-------------------------------------------------------------------------------
// Returns the floating-point tangent of the argument
//-------------------------------------------------------------------------------
inline float IvTan( float a )
{
    return tanf(a);

}  // End of IvTan


//-------------------------------------------------------------------------------
// @ IvSinCos()
//-------------------------------------------------------------------------------
// Returns the floating-point sine and cosine of the argument
//-------------------------------------------------------------------------------
inline void IvSinCos( float a, float& sina, float& cosa )
{
    sina = sinf(a);
    cosa = cosf(a);

}  // End of IvSinCos


//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
