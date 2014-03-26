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

#ifndef __IvRendererD3D9__h__
#define __IvRendererD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "../IvRenderer.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------
class IvShaderProgramD3D9;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvRendererD3D9 : public IvRenderer
{
friend class IvRenderer;

public:
	bool static Create( IDirect3DDevice9* device );

	inline IDirect3DDevice9* GetDevice() { return mDevice; }

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
    bool InitD3D9();
    void BindDefaultShaderIfNeeded(IvVertexFormat format);

    IvShaderProgramD3D9* mShader;

private:
    // constructor/destructor
    IvRendererD3D9( IDirect3DDevice9* device );
    ~IvRendererD3D9();

    // copy operations
    IvRendererD3D9(const IvRendererD3D9& other);
    IvRendererD3D9& operator=(const IvRendererD3D9& other);

private: 
    // private D3D9 things 
	IDirect3DDevice9* mDevice;

	D3DCOLOR 	      mClearColor;
	float			  mClearDepth;
};



//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
