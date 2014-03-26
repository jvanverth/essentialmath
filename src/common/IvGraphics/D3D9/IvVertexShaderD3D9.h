//===============================================================================
// @ IvVertexShaderD3D9.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexShaderD3D9__h__
#define __IvVertexShaderD3D9__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexShader.h"
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

class IvVertexShaderD3D9 : private IvVertexShader
{
public:
    // interface routines

    friend class IvResourceManagerD3D9;
    friend class IvShaderProgramD3D9;
    
private:
    // constructor/destructor
    IvVertexShaderD3D9();
	~IvVertexShaderD3D9();
    
    // initialization
    bool CreateFromFile( const char* filename, IDirect3DDevice9* device );
    bool CreateFromString( const char* filename, IDirect3DDevice9* device );
    bool CreateDefault( IvVertexFormat format, IDirect3DDevice9* device );
    
    void Destroy();
    
private:
    // copy operations
    IvVertexShaderD3D9(const IvVertexShaderD3D9& other);
	IvVertexShaderD3D9& operator=(const IvVertexShaderD3D9& other);

	IDirect3DVertexShader9* mShaderPtr;
    ID3DXConstantTable* mConstantTable;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif