//===============================================================================
// @ IvMatrix44.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvMatrix44__h__
#define __IvMatrix44__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"

#include "IvVector3.h"
#include "IvVector4.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvQuat;
class IvMatrix33;
class IvVector3;

class IvMatrix44
{
public:
    // constructor/destructor
    inline IvMatrix44() { Identity(); }
    inline ~IvMatrix44() {}
    explicit IvMatrix44( const IvQuat& quat );
    explicit IvMatrix44( const IvMatrix33& matrix );
    
    // copy operations
    IvMatrix44(const IvMatrix44& other);
    IvMatrix44& operator=(const IvMatrix44& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvMatrix44& source);

    // accessors
    float &operator()(unsigned int i, unsigned int j);
    float operator()(unsigned int i, unsigned int j) const;
    inline const float* GetFloatPtr() { return mV; }

    // comparison
    bool operator==( const IvMatrix44& other ) const;
    bool operator!=( const IvMatrix44& other ) const;
    bool IsZero() const;
    bool IsIdentity() const;

    // manipulators
    void SetRows( const IvVector4& row1, const IvVector4& row2, 
                  const IvVector4& row3, const IvVector4& row4 ); 
    void GetRows( IvVector4& row1, IvVector4& row2, IvVector4& row3, IvVector4& row4 ); 

    void SetColumns( const IvVector4& col1, const IvVector4& col2, 
                     const IvVector4& col3, const IvVector4& col4 ); 
    void GetColumns( IvVector4& col1, IvVector4& col2, IvVector4& col3, IvVector4& col4 ); 

    void Clean();
    void Identity();

    IvMatrix44& AffineInverse();
    friend IvMatrix44 AffineInverse( const IvMatrix44& mat );

    IvMatrix44& Transpose();
    friend IvMatrix44 Transpose( const IvMatrix44& mat );
        
    // transformations
    IvMatrix44& Translation( const IvVector3& xlate );
    IvMatrix44& Rotation( const IvMatrix33& matrix );
    IvMatrix44& Rotation( const IvQuat& rotate );
    IvMatrix44& Rotation( float zRotation, float yRotation, float xRotation );
    IvMatrix44& Rotation( const IvVector3& axis, float angle );

    IvMatrix44& Scaling( const IvVector3& scale );

    IvMatrix44& RotationX( float angle );
    IvMatrix44& RotationY( float angle );
    IvMatrix44& RotationZ( float angle );

    void GetFixedAngles( float& zRotation, float& yRotation, float& xRotation );
    void GetAxisAngle( IvVector3& axis, float& angle );

    // operators

    // addition and subtraction
    IvMatrix44 operator+( const IvMatrix44& other ) const;
    IvMatrix44& operator+=( const IvMatrix44& other );
    IvMatrix44 operator-( const IvMatrix44& other ) const;
    IvMatrix44& operator-=( const IvMatrix44& other );

    IvMatrix44 operator-() const;

    // multiplication
    IvMatrix44& operator*=( const IvMatrix44& matrix );
    IvMatrix44 operator*( const IvMatrix44& matrix ) const;

    // column vector multiplier
    IvVector4 operator*( const IvVector4& vector ) const;
    // row vector multiplier
    friend IvVector4 operator*( const IvVector4& vector, const IvMatrix44& matrix );

    // scalar multiplication
    IvMatrix44& operator*=( float scalar );
    friend IvMatrix44 operator*( float scalar, const IvMatrix44& matrix );
    IvMatrix44 operator*( float scalar ) const;

    // vector3 ops
    IvVector3 Transform( const IvVector3& point ) const;

    // point ops
    IvVector3 TransformPoint( const IvVector3& point ) const;

    // low-level data accessors - implementation-dependent
    operator float*() { return mV; }
    operator const float*() const { return mV; }

protected:
    // member variables
    float mV[16];

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvMatrix44::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float&
IvMatrix44::operator()(unsigned int i, unsigned int j)
{
   return mV[i + 4*j];

}   // End of IvMatrix44::operator()()

//-------------------------------------------------------------------------------
// @ IvMatrix44::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float
IvMatrix44::operator()(unsigned int i, unsigned int j) const
{
   return mV[i + 4*j];

}   // End of IvMatrix44::operator()()

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
