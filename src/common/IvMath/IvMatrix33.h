//===============================================================================
// @ IvMatrix33.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvMatrix33__h__
#define __IvMatrix33__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvQuat;

class IvMatrix33
{
    friend class IvMatrix44;
public:
    // constructor/destructor
    inline IvMatrix33() {}
    inline ~IvMatrix33() {}
    explicit IvMatrix33( const IvQuat& quat );
    
    // copy operations
    IvMatrix33(const IvMatrix33& other);
    IvMatrix33& operator=(const IvMatrix33& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvMatrix33& source);

    // accessors
    inline float& operator()(unsigned int i, unsigned int j);
    inline float operator()(unsigned int i, unsigned int j) const;

    // comparison
    bool operator==( const IvMatrix33& other ) const;
    bool operator!=( const IvMatrix33& other ) const;
    bool IsZero() const;
    bool IsIdentity() const;

    // manipulators
    void SetRows( const IvVector3& row1, const IvVector3& row2, const IvVector3& row3 ); 
    void GetRows( IvVector3& row1, IvVector3& row2, IvVector3& row3 ) const; 
    IvVector3 GetRow( unsigned int i ) const; 

    void SetColumns( const IvVector3& col1, const IvVector3& col2, const IvVector3& col3 ); 
    void GetColumns( IvVector3& col1, IvVector3& col2, IvVector3& col3 ) const; 
    IvVector3 GetColumn( unsigned int i ) const; 

    void Clean();
    void Identity();

    IvMatrix33& Inverse();
    friend IvMatrix33 Inverse( const IvMatrix33& mat );

    IvMatrix33& Transpose();
    friend IvMatrix33 Transpose( const IvMatrix33& mat );

    // useful computations
    IvMatrix33 Adjoint() const;
    float Determinant() const;
    float Trace() const;
        
    // transformations
    IvMatrix33& Rotation( const IvQuat& rotate );
    IvMatrix33& Rotation( float zRotation, float yRotation, float xRotation );
    IvMatrix33& Rotation( const IvVector3& axis, float angle );

    IvMatrix33& Scaling( const IvVector3& scale );

    IvMatrix33& RotationX( float angle );
    IvMatrix33& RotationY( float angle );
    IvMatrix33& RotationZ( float angle );

    void GetFixedAngles( float& zRotation, float& yRotation, float& xRotation );
    void GetAxisAngle( IvVector3& axis, float& angle );

    // operators

    // addition and subtraction
    IvMatrix33 operator+( const IvMatrix33& other ) const;
    IvMatrix33& operator+=( const IvMatrix33& other );
    IvMatrix33 operator-( const IvMatrix33& other ) const;
    IvMatrix33& operator-=( const IvMatrix33& other );

    IvMatrix33 operator-() const;

    // multiplication
    IvMatrix33& operator*=( const IvMatrix33& matrix );
    IvMatrix33 operator*( const IvMatrix33& matrix ) const;

    // column vector multiplier
    IvVector3 operator*( const IvVector3& vector ) const;
    // row vector multiplier
    friend IvVector3 operator*( const IvVector3& vector, const IvMatrix33& matrix );

    IvMatrix33& operator*=( float scalar );
    friend IvMatrix33 operator*( float scalar, const IvMatrix33& matrix );
    IvMatrix33 operator*( float scalar ) const;

    // low-level data accessors - implementation-dependent
    operator float*() { return mV; }
    operator const float*() const { return mV; }

protected:
    // member variables
    float mV[9];
        
private:
};

IvMatrix33 Inverse( const IvMatrix33& mat );
IvMatrix33 Transpose( const IvMatrix33& mat );

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvMatrix33::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float&
IvMatrix33::operator()(unsigned int i, unsigned int j)
{
   return mV[i + 3*j];

}   // End of IvMatrix33::operator()()

//-------------------------------------------------------------------------------
// @ IvMatrix33::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float
IvMatrix33::operator()(unsigned int i, unsigned int j) const
{
   return mV[i + 3*j];

}   // End of IvMatrix33::operator()()

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
