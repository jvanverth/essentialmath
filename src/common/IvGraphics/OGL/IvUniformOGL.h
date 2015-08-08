//===============================================================================
// @ IvUniformOGL.h
// 
// OpenGL implementation for uniform/constant shader variable
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvUniformOGL__h__
#define __IvUniformOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvUniform.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvMatrix44;
class IvTextureOGL;
class IvVector3;
class IvVector4;
class IvShaderProgramOGL;
class IvTexture;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvUniformOGL : public IvUniform
{
public:
    friend class IvShaderProgramOGL;

    void SetValue(float value, unsigned int index) final;
    void SetValue(const IvVector3& value, unsigned int index) final;
    void SetValue(const IvVector4& value, unsigned int index) final;
    void SetValue(const IvMatrix44& value, unsigned int index) final;
    void SetValue(IvTexture* value) final;
    
    // return false on type mismatch
    bool GetValue(float& value, unsigned int index) const final;
    bool GetValue(IvVector3& value, unsigned int index) const final;
    bool GetValue(IvVector4& value, unsigned int index) const final;
    bool GetValue(IvMatrix44& value, unsigned int index) const final;
    bool GetValue(IvTexture*& value) const final;
    
protected:
    // constructor/destructor
    IvUniformOGL(IvUniformType type, unsigned int count, 
                 IvShaderProgramOGL* shader, unsigned int shaderIndex,
                 unsigned int textureStage);
    virtual ~IvUniformOGL() final;
    void Update();

    IvShaderProgramOGL* mShader;
    unsigned int mShaderIndex;

    // Could subclass for each type, but likely not worth it.
    union  
    {
        float* mFloat;
        IvVector3* mVector3;
        IvVector4* mVector4;
        IvMatrix44* mMatrix44;
        IvTextureOGL* mTexture;
    } mValue;

    int mTextureStage;

private:
    // copy operations (unimplemented so we can't copy)
    IvUniformOGL(const IvUniformOGL& other);
    IvUniformOGL& operator=(const IvUniformOGL& other);
}; 

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
