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

#ifndef __IvRendererD3D11__h__
#define __IvRendererD3D11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "../IvRenderer.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------
class IvShaderProgramD3D11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvRendererD3D11 : public IvRenderer
{
friend class IvRenderer;

public:
    bool static Create(ID3D11Device* device, ID3D11DeviceContext* context,
                       ID3D11RenderTargetView* renderTarget, 
                       ID3D11DepthStencilView* depthStencilTarget);

    bool Initialize( unsigned int  width, unsigned int  height ) final;
    void Resize( unsigned int width, unsigned int height ) final;
    void SetTargets(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilTarget);
    void ReleaseTargets();

    void SetClearColor( float red, float green, float blue, float alpha ) final;
    void SetClearDepth( float depth ) final;
    void ClearBuffers(IvClearBuffer buffer) final;

    void SetBlendFunc(IvBlendFunc srcBlend, IvBlendFunc dstBlend, IvBlendOp op) final;
    void SetColorMask(bool red, bool green, bool blue, bool alpha) final;

    void SetFillMode(IvFillMode fill) final;
    void SetShadeMode(IvShadeMode shade) final;
    IvShadeMode GetShadeMode() final;

    void SetDepthTest(IvDepthTestFunc func) final;
    IvDepthTestFunc GetDepthTest() final;
    void SetDepthWrite(bool write) final;

    const IvMatrix44& GetWorldMatrix();
    void SetWorldMatrix(const IvMatrix44& matrix) final;
    const IvMatrix44& GetViewMatrix();
    void SetViewMatrix(const IvMatrix44& matrix) final;
    const IvMatrix44& GetProjectionMatrix();
    void SetProjectionMatrix(const IvMatrix44& matrix) final;

    // Setting this to NULL uses the default shader for the vertex format
    // that is drawn
    // However, if NULL is set, Get will return NULL, as the default shader cannot
    // be determined without a vertex format
    IvShaderProgram* GetShaderProgram() final;
    void SetShaderProgram(IvShaderProgram* program) final;
    
    void Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
              IvIndexBuffer* indexBuffer, unsigned int numIndices) final;
    void Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices) final;

    ID3D11Device* GetDevice()         { return mDevice; }
    ID3D11DeviceContext* GetContext() { return mContext; }
    
protected:
    bool InitD3D11();
    void BindDefaultShaderIfNeeded(IvVertexFormat format);
    void UpdateUniforms();

    IvShaderProgramD3D11* mShader;

private:
    // constructor/destructor
    IvRendererD3D11(ID3D11Device* device, ID3D11DeviceContext* context, 
                    ID3D11RenderTargetView* renderTarget, 
                    ID3D11DepthStencilView* depthStencilTarget);
    ~IvRendererD3D11() final;

    // copy operations
    IvRendererD3D11(const IvRendererD3D11& other);
    IvRendererD3D11& operator=(const IvRendererD3D11& other);

    void UpdateDepthStencilState();
    void UpdateBlendState();

    // private D3D11 things 
    ID3D11Device*        mDevice;
    ID3D11DeviceContext* mContext;
    ID3D11RenderTargetView* mRenderTarget;
    ID3D11DepthStencilView* mDepthStencilTarget;

    IvDepthTestFunc      mDepthTestFunc;
    BOOL                 mDepthWrite;

    float                mClearColor[4];
    float                mClearDepth;

    D3D11_BLEND          mSrcBlend;
    D3D11_BLEND          mDestBlend;
    D3D11_BLEND          mSrcBlendAlpha;
    D3D11_BLEND          mDestBlendAlpha;
    D3D11_BLEND_OP       mBlendOp;
    UINT8                mWriteMask;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
