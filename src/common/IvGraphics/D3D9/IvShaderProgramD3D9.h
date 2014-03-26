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

#include <d3d9.h>
#include <D3DX9Shader.h>

#include "../IvShaderProgram.h"
#include "../IvVertexFormats.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvResourceManagerD3D9;
class IvUniformD3D9;
class IvVertexShaderD3D9;
class IvFragmentShaderD3D9;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvShaderProgramD3D9 : public IvShaderProgram
{
public:
    // interface routines
    virtual IvUniform* GetUniform(char const* name);

    friend class IvResourceManagerD3D9;
    friend class IvRendererD3D9;
    
private:
    // constructor/destructor
    IvShaderProgramD3D9();
	~IvShaderProgramD3D9();
    
    // initialization
    bool Create( IvVertexShaderD3D9* vertexShaderPtr, IvFragmentShaderD3D9* fragmentShaderPtr );
    
    // destruction
    void Destroy();
    
    // make this the active program
    bool MakeActive( IDirect3DDevice9* device );
    
private:
    // copy operations
    IvShaderProgramD3D9(const IvShaderProgramD3D9& other);
	IvShaderProgramD3D9& operator=(const IvShaderProgramD3D9& other);

private:
    // D3D-specific data
	IDirect3DVertexShader9* mVertexShaderPtr;
    ID3DXConstantTable*     mVertexShaderConstants;
 	IDirect3DPixelShader9*  mFragmentShaderPtr;
    ID3DXConstantTable*     mFragmentShaderConstants;

    std::map<std::string, IvUniformD3D9*> mUniforms;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
