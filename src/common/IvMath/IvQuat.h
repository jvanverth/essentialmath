//===============================================================================
// @ IvQuat.h
// 
// Quaternion class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvQuat__h__
#define __IvQuat__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class IvMatrix33;

class IvQuat
{
    friend class IvMatrix33;
    friend class IvMatrix44;
    
public:
    // constructor/destructor
    inline IvQuat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {}
    inline IvQuat( float _w, float _x, float _y, float _z ) :
        w(_w), x(_x), y(_y), z(_z)
    {
    }
    IvQuat( const IvVector3& axis, float angle );
    IvQuat( const IvVector3& from, const IvVector3& to );
    explicit IvQuat( const IvVector3& vector );
    explicit IvQuat( const IvMatrix33& rotation );
    inline ~IvQuat() {}

    // copy operations
    IvQuat(const IvQuat& other);
    IvQuat& operator=(const IvQuat& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvQuat& source);

    // accessors
    inline float& operator[]( unsigned int i )         { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const    { return (&x)[i]; }

    float Magnitude() const;
    float Norm() const;

    // comparison
    bool operator==( const IvQuat& other ) const;
    bool operator!=( const IvQuat& other ) const;
    bool IsZero() const;
    bool IsUnit() const;
    bool IsIdentity() const;

    // manipulators
    inline void Set( float _w, float _x, float _y, float _z );
    void Set( const IvVector3& axis, float angle );
    void Set( const IvVector3& from, const IvVector3& to );
    void Set( const IvMatrix33& rotation );
    void Set( float zRotation, float yRotation, float xRotation ); 

    void GetAxisAngle( IvVector3& axis, float& angle );

    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit quaternion
    inline void Identity();    // sets to identity quaternion

    // complex conjugate
    friend IvQuat Conjugate( const IvQuat& quat );
    const IvQuat& Conjugate();

    // invert quaternion
    friend IvQuat Inverse( const IvQuat& quat );
    const IvQuat& Inverse();

    // operators

    // addition/subtraction
    IvQuat operator+( const IvQuat& other ) const;
    IvQuat& operator+=( const IvQuat& other );
    IvQuat operator-( const IvQuat& other ) const;
    IvQuat& operator-=( const IvQuat& other );

    IvQuat operator-() const;

    // scalar multiplication
    friend IvQuat    operator*( float scalar, const IvQuat& vector );
    IvQuat&          operator*=( float scalar );

    // quaternion multiplication
    IvQuat operator*( const IvQuat& other ) const;
    IvQuat& operator*=( const IvQuat& other );

    // dot product
    float              Dot( const IvQuat& vector ) const;
    friend float       Dot( const IvQuat& vector1, const IvQuat& vector2 );

    // vector rotation
    IvVector3   Rotate( const IvVector3& vector ) const;

    // interpolation
    friend void Lerp( IvQuat& result, const IvQuat& start, const IvQuat& end, float t );
    friend void Slerp( IvQuat& result, const IvQuat& start, const IvQuat& end, float t );
    friend void ApproxSlerp( IvQuat& result, const IvQuat& start, const IvQuat& end, float t );

    // useful defaults
    static IvQuat   zero;
    static IvQuat   identity;
        
protected:
    // member variables
    float w, x, y, z;      

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvQuat::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
IvQuat::Set( float _w, float _x, float _y, float _z )
{
    w = _w; x = _x; y = _y; z = _z;
}   // End of IvQuat::Set()

//-------------------------------------------------------------------------------
// @ IvQuat::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
IvQuat::Zero()
{
    x = y = z = w = 0.0f;
}   // End of IvQuat::Zero()

//-------------------------------------------------------------------------------
// @ IvQuat::Identity()
//-------------------------------------------------------------------------------
// Set to identity quaternion
//-------------------------------------------------------------------------------
inline void 
IvQuat::Identity()
{
    x = y = z = 0.0f;
    w = 1.0f;
}   // End of IvQuat::Identity

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
