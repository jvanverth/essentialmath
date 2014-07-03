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

#ifndef __IvRendererDX11__h__
#define __IvRendererDX11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "../IvRenderer.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------
class IvShaderProgramDX11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvRendererDX11 : public IvRenderer
{
friend class IvRenderer;

public:
	bool static Create(ID3D11Device* device, ID3D11DeviceContext* context,
		               ID3D11RenderTargetView* renderTarget, 
					   ID3D11DepthStencilView* depthStencilTarget);

    virtual bool Initialize( unsigned int  width, unsigned int  height );
    virtual void Resize( unsigned int width, unsigned int height );
        
    virtual void  SetClearColor( float red, float green, float blue, float alpha );
	virtual void  SetClearDepth( float depth );
    virtual void  ClearBuffers(IvClearBuffer buffer);

    virtual void SetBlendFunc(IvBlendFunc blend);
	virtual void SetColorMask( bool red, bool green, bool blue, bool alpha );
	virtual void SetFillMode(IvFillMode fill);
	virtual void SetShadeMode(IvShadeMode shade);
	virtual IvShadeMode GetShadeMode();

	virtual void SetDepthTest(IvDepthTestFunc func);
    virtual IvDepthTestFunc GetDepthTest();
    virtual void SetDepthWrite(bool write);

    const IvMatrix44& GetWorldMatrix();
    virtual void SetWorldMatrix(const IvMatrix44& matrix);
    const IvMatrix44& GetViewMatrix();
    virtual void SetViewMatrix(const IvMatrix44& matrix);
    const IvMatrix44& GetProjectionMatrix();
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
    bool InitDX11();
    void BindDefaultShaderIfNeeded(IvVertexFormat format);

    IvShaderProgramDX11* mShader;

private:
    // constructor/destructor
	IvRendererDX11(ID3D11Device* device, ID3D11DeviceContext* context, 
		           ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilTarget);
    ~IvRendererDX11();

    // copy operations
    IvRendererDX11(const IvRendererDX11& other);
    IvRendererDX11& operator=(const IvRendererDX11& other);

private: 
    // private DX11 things 
	ID3D11Device*        mDevice;
	ID3D11DeviceContext* mContext;
	ID3D11RenderTargetView* mRenderTarget;
	ID3D11DepthStencilView* mDepthStencilTarget;

	float 	             mClearColor[4];
	float			     mClearDepth;
};



//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
