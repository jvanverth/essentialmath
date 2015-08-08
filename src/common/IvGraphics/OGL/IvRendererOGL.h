//===============================================================================
// @ IvRenderer.h
// 
// Windowing routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//
//
// Virtual base class for platform-dependant windowing
//===============================================================================

#ifndef __IvRendererOGL__h__
#define __IvRendererOGL__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "../IvRenderer.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------
class IvShaderProgramOGL;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvRendererOGL : public IvRenderer
{
friend class IvRenderer;

public:
    bool static Create();

    bool Initialize(unsigned int  width, unsigned int  height) final;
    void Resize(unsigned int width, unsigned int height) final;
        
    void  SetClearColor(float red, float green, float blue, float alpha) final;
    void  SetClearDepth(float depth) final;
    void  ClearBuffers(IvClearBuffer buffer) final;

    void SetBlendFunc(IvBlendFunc srcBlend, IvBlendFunc dstBlend, IvBlendOp op) final;
    void SetColorMask(bool red, bool green, bool blue, bool alpha) final;
    void SetFillMode(IvFillMode fill) final;
    void SetShadeMode(IvShadeMode shade) final;
    IvShadeMode GetShadeMode() final;

    void SetDepthTest(IvDepthTestFunc func) final;
    IvDepthTestFunc GetDepthTest() final;
    void SetDepthWrite(bool write) final;

    void SetWorldMatrix(const IvMatrix44& matrix) final;
    void SetViewMatrix(const IvMatrix44& matrix) final;
    void SetProjectionMatrix(const IvMatrix44& matrix) final;

    // Setting this to nullptr uses the default shader for the vertex format
    // that is drawn
    // However, if nullptr is set, Get will return nullptr, as the default shader cannot
    // be determined without a vertex format
    IvShaderProgram* GetShaderProgram() final;
    void SetShaderProgram(IvShaderProgram* program) final;
    
    void Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
                      IvIndexBuffer* indexBuffer, unsigned int numIndices) final;
    void Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices) final;
    
protected:
    int InitGL(void);
    void BindDefaultShaderIfNeeded(IvVertexFormat format);

    IvShaderProgramOGL* mShader;

private:
    // constructor/destructor
    IvRendererOGL();
    ~IvRendererOGL() final;

    // copy operations
    IvRendererOGL(const IvRendererOGL& other);
    IvRendererOGL& operator=(const IvRendererOGL& other);
};



//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
