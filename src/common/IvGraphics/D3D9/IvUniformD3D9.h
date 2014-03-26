//===============================================================================
// @ IvUniformD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvUniformD3D9__h__
#define __IvUniformD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvUniform.h"
#include <d3d9.h>
#include <D3DX9Shader.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvMatrix44;
class IvTextureD3D9;
class IvVector4;
class IvShaderProgramD3D9;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvUniformD3D9 : public IvUniform
{
public:
    friend class IvShaderProgramD3D9;

    virtual void SetValue( float value, unsigned int index);
    virtual void SetValue( const IvVector4& value, unsigned int index );
    virtual void SetValue( const IvMatrix44& value, unsigned int index );
    virtual void SetValue( IvTexture* value );
    
    // return false on type mismatch
    virtual bool GetValue( float& value, unsigned int index) const;
    virtual bool GetValue( IvVector4& value, unsigned int index ) const;
    virtual bool GetValue( IvMatrix44& value, unsigned int index ) const;
    virtual bool GetValue( IvTexture*& value ) const;
    
protected:
    // constructor/destructor
    IvUniformD3D9(IvUniformType type, unsigned int count, D3DXHANDLE uniform, 
				  LPD3DXCONSTANTTABLE constantTable, IvShaderProgramD3D9* shader);
    virtual ~IvUniformD3D9();
    void Update();

    IvShaderProgramD3D9*	mShader;
    D3DXHANDLE				mUniform;
    LPD3DXCONSTANTTABLE		mConstantTable;
    bool					mNeedsUpdate;

    // Could subclass for each type, but likely not worth it.
    union  
    {
        float* mFloat;
        IvVector4* mVector4;
        IvMatrix44* mMatrix44;
        IvTextureD3D9* mTexture;
    } mValue;

private:
    // copy operations (unimplemented so we can't copy)
    IvUniformD3D9(const IvUniformD3D9& other);
	IvUniformD3D9& operator=(const IvUniformD3D9& other);
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
