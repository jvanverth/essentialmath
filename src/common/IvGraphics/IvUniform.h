//===============================================================================
// @ IvUniform.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Usage notes
//===============================================================================

#ifndef __IvUniform__h__
#define __IvUniform__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvMatrix44;
class IvTexture;
class IvVector3;
class IvVector4;
class IvTexture;

// To avoid an explosion of types, we only support 1D and 4D vectors and
// 4x4 matrices
enum IvUniformType 
{
    kFloatUniform,
    kFloat3Uniform,
    kFloat4Uniform,
    kFloatMatrix44Uniform,
    kTextureUniform
};

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvUniform
{
public:
    inline IvUniformType GetType() { return mType; }

    inline unsigned int GetCount() { return mCount; }

    virtual void SetValue( float value, unsigned int index) = 0;
    virtual void SetValue( const IvVector3& value, unsigned int index ) = 0;
    virtual void SetValue( const IvVector4& value, unsigned int index ) = 0;
    virtual void SetValue( const IvMatrix44& value, unsigned int index ) = 0;
    virtual void SetValue( IvTexture* value ) = 0;
    
    // return false on type mismatch
    virtual bool GetValue( float& value, unsigned int index) const = 0;
    virtual bool GetValue( IvVector3& value, unsigned int index ) const = 0;
    virtual bool GetValue( IvVector4& value, unsigned int index ) const = 0;
    virtual bool GetValue( IvMatrix44& value, unsigned int index ) const = 0;
    virtual bool GetValue( IvTexture*& value ) const = 0;
    
protected:
    // constructor/destructor
    IvUniform( IvUniformType type, unsigned int count ) : mType(type), mCount(count), mNeedsUpdate(true) {}
    virtual ~IvUniform() {}

    IvUniformType   mType;
    unsigned int    mCount;
    bool            mNeedsUpdate;

private:
    // copy operations (unimplemented so we can't copy)
    IvUniform(const IvUniform& other);
    IvUniform& operator=(const IvUniform& other);
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
