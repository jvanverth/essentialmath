//===============================================================================
// @ IvFragmentShaderD3D11.h
// 
// D3D11 fragment shader implementation
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015   James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

#ifndef __IvFragmentShaderD3D11__h__
#define __IvFragmentShaderD3D11__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "../IvFragmentShader.h"
#include "../IvVertexFormats.h"
#include <d3d11.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvConstantTableD3D11;
class IvResourceManagerD3D11;
class IvShaderProgramD3D11;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvFragmentShaderD3D11 : private IvFragmentShader
{
public:
    // interface routines

    friend class IvResourceManagerD3D11;
    friend class IvShaderProgramD3D11;

private:
    // constructor/destructor
    IvFragmentShaderD3D11();
    ~IvFragmentShaderD3D11() final;
    
    // initialization
    bool CreateFromFile(const char* filename, ID3D11Device* device);
    bool CreateFromString(const char* filename, ID3D11Device* device);
    bool CreateDefault(IvVertexFormat format, ID3D11Device* device);
    
    void Destroy();
    
private:
    // copy operations
    IvFragmentShaderD3D11(const IvFragmentShaderD3D11& other);
    IvFragmentShaderD3D11& operator=(const IvFragmentShaderD3D11& other);

    ID3D11PixelShader*   mShaderPtr;
    IvConstantTableD3D11* mConstantTable;
};


//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif