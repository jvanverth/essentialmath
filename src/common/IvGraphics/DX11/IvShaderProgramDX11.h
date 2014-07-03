//===============================================================================
// @ IvShaderProgramD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvShaderProgramD3D9__h__
#define __IvShaderProgramD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <map>
#include <string>

#include <d3d11.h>
//#include <D3DX9Shader.h>

#include "../IvShaderProgram.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvConstantTableDX11;
class IvResourceManagerDX11;
class IvUniformDX11;
class IvVertexShaderDX11;
class IvFragmentShaderDX11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvShaderProgramDX11 : public IvShaderProgram
{
public:
    // interface routines
    virtual IvUniform* GetUniform(char const* name);

    friend class IvResourceManagerDX11;
    friend class IvRendererDX11;
    
private:
    // constructor/destructor
    IvShaderProgramDX11();
	~IvShaderProgramDX11();
    
    // initialization
    bool Create( IvVertexShaderDX11* vertexShaderPtr, IvFragmentShaderDX11* fragmentShaderPtr );
    
    // destruction
    void Destroy();
    
    // make this the active program
	bool MakeActive(ID3D11DeviceContext* context);

	// bind the associated uniforms
	bool BindUniforms(ID3D11DeviceContext* context);
    
private:
    // copy operations
    IvShaderProgramDX11(const IvShaderProgramDX11& other);
	IvShaderProgramDX11& operator=(const IvShaderProgramDX11& other);

private:
    // D3D-specific data
	ID3D11VertexShader*  mVertexShaderPtr;
	IvConstantTableDX11* mVertexShaderConstants;
	ID3D11PixelShader*   mFragmentShaderPtr;
	IvConstantTableDX11* mFragmentShaderConstants;

    std::map<std::string, IvUniformDX11*> mUniforms;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
