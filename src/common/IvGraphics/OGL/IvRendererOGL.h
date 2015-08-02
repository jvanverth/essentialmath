//===============================================================================
// @ IvRenderer.h
// 
// Windowing routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
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

    virtual bool Initialize( unsigned int  width, unsigned int  height );
    virtual void Resize( unsigned int width, unsigned int height );
        
    virtual void  SetClearColor( float red, float green, float blue, float alpha );
    virtual void  SetClearDepth( float depth );
    virtual void  ClearBuffers(IvClearBuffer buffer);

    virtual void SetBlendFunc(IvBlendFunc srcBlend, IvBlendFunc dstBlend, IvBlendOp op);
    virtual void SetColorMask( bool red, bool green, bool blue, bool alpha );
    virtual void SetFillMode(IvFillMode fill);
    virtual void SetShadeMode(IvShadeMode shade);
    virtual IvShadeMode GetShadeMode();

    virtual void SetDepthTest(IvDepthTestFunc func);
    virtual IvDepthTestFunc GetDepthTest();
    virtual void SetDepthWrite(bool write);

    virtual void SetWorldMatrix(const IvMatrix44& matrix);
    virtual void SetViewMatrix(const IvMatrix44& matrix);
    virtual void SetProjectionMatrix(const IvMatrix44& matrix);

    // Setting this to NULL uses the default shader for the vertex format
    // that is drawn
    // However, if NULL is set, Get will return NULL, as the default shader cannot
    // be determined without a vertex format
    virtual IvShaderProgram* GetShaderProgram();
    virtual void SetShaderProgram(IvShaderProgram* program);
    
    virtual void Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
                      IvIndexBuffer* indexBuffer, unsigned int numIndices);
    virtual void Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices);
    
protected:
    int InitGL(void);
    void BindDefaultShaderIfNeeded(IvVertexFormat format);

    IvShaderProgramOGL* mShader;

private:
    // constructor/destructor
    IvRendererOGL();
    ~IvRendererOGL();

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
