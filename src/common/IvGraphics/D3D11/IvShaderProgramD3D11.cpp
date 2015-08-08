//===============================================================================
// @ IvShaderProgramD3D11.cpp
// 
// D3D11 implementation for combined vertex and fragment/pixel shader
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015  James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvShaderProgramD3D11.h"
#include "IvAssert.h"
#include "IvConstantTableD3D11.h"
#include "IvFragmentShaderD3D11.h"
#include "IvUniformD3D11.h"
#include "IvVertexShaderD3D11.h"

// probably don't belong here
//#include "IvMatrix44.h"
//#include "IvRendererD3D11.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::IvShaderProgramD3D11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvShaderProgramD3D11::IvShaderProgramD3D11() : 
    IvShaderProgram(), 
    mVertexShaderPtr(0), 
    mFragmentShaderPtr(0),
    mVertexShaderConstants(0),
    mFragmentShaderConstants(0)
{
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::~IvShaderProgramD3D11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvShaderProgramD3D11::~IvShaderProgramD3D11()
{
    ASSERT(!mVertexShaderPtr);
    ASSERT(!mFragmentShaderPtr);
    ASSERT(!mVertexShaderConstants);
    ASSERT(!mFragmentShaderConstants);
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::Create()
//-------------------------------------------------------------------------------
// Create a shader program
//-------------------------------------------------------------------------------
bool
IvShaderProgramD3D11::Create( IvVertexShaderD3D11* vertexShaderPtr, IvFragmentShaderD3D11* fragmentShaderPtr )
{
    // check for valid inputs
    if ( 0 == vertexShaderPtr || 0 == fragmentShaderPtr )
        return false;
    
    mVertexShaderPtr = vertexShaderPtr->mShaderPtr; 
    mVertexShaderPtr->AddRef();
    mVertexShaderConstants = vertexShaderPtr->mConstantTable;
    mVertexShaderConstants->AddRef();

    mFragmentShaderPtr = fragmentShaderPtr->mShaderPtr;
    mFragmentShaderPtr->AddRef();
    mFragmentShaderConstants = fragmentShaderPtr->mConstantTable;
    mFragmentShaderConstants->AddRef();

    return true;
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvShaderProgramD3D11::Destroy()
{
    if (mVertexShaderPtr)
    {
        mVertexShaderPtr->Release();
        mVertexShaderPtr = 0;
    }
    if (mVertexShaderConstants)
    {
        mVertexShaderConstants->Release();
        mVertexShaderConstants = 0;
    }

    if (mFragmentShaderPtr)
    {
        mFragmentShaderPtr->Release();
        mFragmentShaderPtr = 0;
    }
    if (mFragmentShaderConstants)
    {
        mFragmentShaderConstants->Release();
        mFragmentShaderConstants = 0;
    }

    std::map<std::string, IvUniformD3D11*>::iterator iter = mUniforms.begin();
    while (iter != mUniforms.end())
    {
        delete iter->second;
        iter++;
    }
    mUniforms.clear();
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active program
//-------------------------------------------------------------------------------
bool
IvShaderProgramD3D11::MakeActive(ID3D11DeviceContext* context)
{
    if ( mVertexShaderPtr == 0 || mFragmentShaderPtr == 0 )
        return false;
    
    context->VSSetShader(mVertexShaderPtr, nullptr, 0);
    context->PSSetShader(mFragmentShaderPtr, nullptr, 0);

    //*** Update the uniforms here?

    std::map<std::string, IvUniformD3D11*>::iterator iter = mUniforms.begin();
   
    while (iter != mUniforms.end())
    {
        iter->second->Update();
        
        iter++;
    }

    return true;
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::BindUniforms()
//-------------------------------------------------------------------------------
// Bind the associated uniforms
//-------------------------------------------------------------------------------
bool
IvShaderProgramD3D11::BindUniforms(ID3D11DeviceContext* context)
{
    if (mVertexShaderConstants == 0 || mFragmentShaderConstants == 0)
        return false;

    mVertexShaderConstants->MakeActiveVS(context);
    mFragmentShaderConstants->MakeActivePS(context);

    return true;
}


//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D11::GetUniform()
//-------------------------------------------------------------------------------
// Queries, returns and caches a shader uniform
//-------------------------------------------------------------------------------
IvUniform*
IvShaderProgramD3D11::GetUniform(char const* name)
{
    // Did we already query the uniform?
    if (mUniforms.find(name) != mUniforms.end())
    {
        return static_cast<IvUniform*>(mUniforms[name]);
    }

    IvConstantDesc desc;

    // check the vertex shader first
    IvConstantTableD3D11* constantTable;
    if (mVertexShaderConstants->GetConstantDesc(name, &desc))
    {
        constantTable = mVertexShaderConstants;
    } 
    else if (mFragmentShaderConstants->GetConstantDesc(name, &desc))
    {
        constantTable = mFragmentShaderConstants;
    }
    else
    {
        return nullptr;
    }

    IvUniformD3D11* uniform;
    if (desc.mType == kTextureUniform)
    {
        uniform = new IvUniformD3D11(desc.mTextureSlot, desc.mSamplerSlot, this);
    } 
    else
    {
        uniform = new IvUniformD3D11(desc.mType, desc.mCount, desc.mOffset, constantTable, this);
    }

    mUniforms[name] = uniform;

    return uniform;
}
