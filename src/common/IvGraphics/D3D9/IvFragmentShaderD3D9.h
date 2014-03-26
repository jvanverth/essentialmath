//===============================================================================
// @ IvFragmentShaderD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvFragmentShaderD3D9__h__
#define __IvFragmentShaderD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvFragmentShader.h"
#include "../IvVertexFormats.h"
#include <d3d9.h>
#include <D3DX9Shader.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvResourceManagerD3D9;
class IvShaderProgramD3D9;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvFragmentShaderD3D9 : private IvFragmentShader
{
public:
    // interface routines

    friend class IvResourceManagerD3D9;
    friend class IvShaderProgramD3D9;

private:
    // constructor/destructor
    IvFragmentShaderD3D9();
	~IvFragmentShaderD3D9();
    
    // initialization
    bool CreateFromFile( const char* filename, IDirect3DDevice9* device  );
    bool CreateFromString( const char* filename, IDirect3DDevice9* device  );
    bool CreateDefault( IvVertexFormat format, IDirect3DDevice9* device  );
    
    void Destroy();
    
private:
    // copy operations
    IvFragmentShaderD3D9(const IvFragmentShaderD3D9& other);
	IvFragmentShaderD3D9& operator=(const IvFragmentShaderD3D9& other);

	IDirect3DPixelShader9* mShaderPtr;
    ID3DXConstantTable* mConstantTable;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif