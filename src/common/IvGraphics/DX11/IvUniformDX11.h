//===============================================================================
// @ IvUniformDX11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Usage notes
//===============================================================================

#ifndef __IvUniformDX11__h__
#define __IvUniformDX11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvUniform.h"
#include "IvTypes.h"
#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvConstantTableDX11;
class IvMatrix44;
class IvTextureDX11;
class IvVector4;
class IvShaderProgramDX11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvUniformDX11 : public IvUniform
{
public:
    friend class IvShaderProgramDX11;

    virtual void SetValue( float value, unsigned int index);
    virtual void SetValue( const IvVector4& value, unsigned int index );
	virtual void SetValue(const IvVector3& value, unsigned int index);
	virtual void SetValue(const IvMatrix44& value, unsigned int index);
    virtual void SetValue( IvTexture* value );
    
    // return false on type mismatch
    virtual bool GetValue( float& value, unsigned int index) const;
    virtual bool GetValue( IvVector4& value, unsigned int index ) const;
	virtual bool GetValue(IvVector3& value, unsigned int index) const;
	virtual bool GetValue(IvMatrix44& value, unsigned int index) const;
    virtual bool GetValue( IvTexture*& value ) const;
    
protected:
    // constructor/destructor for most uniforms
    IvUniformDX11(IvUniformType type, unsigned int count, void* offset,
		          IvConstantTableDX11* constantTable, 
				  IvShaderProgramDX11* shader);
	// constructor/destructor for texture uniforms
	IvUniformDX11(int textureUnit, int samplerUnit,
		          IvShaderProgramDX11* shader);
	virtual ~IvUniformDX11();
    void Update();

    IvShaderProgramDX11*	 mShader;
	union
	{
        // we use an offset into the global constant buffer
        // to update most uniforms
        struct
        {
            void*				 mOffset;
            IvConstantTableDX11* mConstantTable;
        };
        // we use the texture and sampler units for textures
		struct
		{
			int                  mTextureUnit;
			int                  mSamplerUnit;
		};
	};

    // Could subclass for each type (particularly textures), 
    // but likely not worth it.
    union  
    {
        float* mFloat;
		IvVector3* mVector3;
		IvVector4* mVector4;
		IvMatrix44* mMatrix44;
        IvTextureDX11* mTexture;
    } mValue;

private:
    // copy operations (unimplemented so we can't copy)
    IvUniformDX11(const IvUniformDX11& other);
	IvUniformDX11& operator=(const IvUniformDX11& other);
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
