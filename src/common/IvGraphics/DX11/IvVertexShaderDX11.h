//===============================================================================
// @ IvVertexShaderDX11.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008   Elsevier, Inc.
//
// Change history:
//
// Usage notes
//===============================================================================

#ifndef __IvVertexShaderDX11__h__
#define __IvVertexShaderDX11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvVertexShader.h"
#include "../IvVertexFormats.h"
#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvResourceManagerDX11;
class IvShaderProgramDX11;
class IvConstantTableDX11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVertexShaderDX11 : private IvVertexShader
{
public:
    // interface routines

    friend class IvResourceManagerDX11;
    friend class IvShaderProgramDX11;
    
private:
    // constructor/destructor
    IvVertexShaderDX11();
	~IvVertexShaderDX11();
    
    // initialization
	bool CreateFromFile(const char* filename, ID3D11Device* device);
	bool CreateFromString(const char* filename, ID3D11Device* device);
	bool CreateDefault(IvVertexFormat format, ID3D11Device* device);
    
    void Destroy();
    
private:
    // copy operations
    IvVertexShaderDX11(const IvVertexShaderDX11& other);
	IvVertexShaderDX11& operator=(const IvVertexShaderDX11& other);

	ID3D11VertexShader*  mShaderPtr;
	IvConstantTableDX11* mConstantTable;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif