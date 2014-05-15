//===============================================================================
// @ IvFragmentShaderDX11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvFragmentShaderDX11__h__
#define __IvFragmentShaderDX11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvFragmentShader.h"
#include "../IvVertexFormats.h"
#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvConstantTableDX11;
class IvResourceManagerDX11;
class IvShaderProgramDX11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvFragmentShaderDX11 : private IvFragmentShader
{
public:
    // interface routines

    friend class IvResourceManagerDX11;
    friend class IvShaderProgramDX11;

private:
    // constructor/destructor
    IvFragmentShaderDX11();
	~IvFragmentShaderDX11();
    
    // initialization
	bool CreateFromFile(const char* filename, ID3D11Device* device);
	bool CreateFromString(const char* filename, ID3D11Device* device);
	bool CreateDefault(IvVertexFormat format, ID3D11Device* device);
    
    void Destroy();
    
private:
    // copy operations
    IvFragmentShaderDX11(const IvFragmentShaderDX11& other);
	IvFragmentShaderDX11& operator=(const IvFragmentShaderDX11& other);

	ID3D11PixelShader*   mShaderPtr;
	IvConstantTableDX11* mConstantTable;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif