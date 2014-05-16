//===============================================================================
// @ IvShaderProgramDX11.cpp
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008  Elsevier, Inc.
//
// Implementation notes
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvShaderProgramDX11.h"
#include "IvConstantTableDX11.h"
#include "IvFragmentShaderDX11.h"
#include "IvUniformDX11.h"
#include "IvVertexShaderDX11.h"

// probably don't belong here
#include "IvMatrix44.h"
#include "IvRendererDX11.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvShaderProgramDX11::IvShaderProgramDX11()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvShaderProgramDX11::IvShaderProgramDX11() : 
    IvShaderProgram(), 
    mVertexShaderPtr(0), 
    mFragmentShaderPtr(0),
    mVertexShaderConstants(0),
    mFragmentShaderConstants(0)
{
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramDX11::~IvShaderProgramDX11()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvShaderProgramDX11::~IvShaderProgramDX11()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramDX11::Create()
//-------------------------------------------------------------------------------
// Create a shader program
//-------------------------------------------------------------------------------
bool
IvShaderProgramDX11::Create( IvVertexShaderDX11* vertexShaderPtr, IvFragmentShaderDX11* fragmentShaderPtr )
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
// @ IvShaderProgramDX11::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvShaderProgramDX11::Destroy()
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

    std::map<std::string, IvUniformDX11*>::iterator iter = mUniforms.begin();
    while (iter != mUniforms.end())
    {
        delete iter->second;
        iter++;
    }
    mUniforms.clear();
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramDX11::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active program
//-------------------------------------------------------------------------------
bool
IvShaderProgramDX11::MakeActive(ID3D11DeviceContext* context)
{
    if ( mVertexShaderPtr == 0 || mFragmentShaderPtr == 0 )
        return false;
	
	context->VSSetShader(mVertexShaderPtr, NULL, 0);
	context->PSSetShader(mFragmentShaderPtr, NULL, 0);

	//*** Update the uniforms here?

    std::map<std::string, IvUniformDX11*>::iterator iter = mUniforms.begin();
   
    while (iter != mUniforms.end())
    {
        iter->second->Update();
        
        iter++;
    }

    return true;
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramDX11::GetUniform()
//-------------------------------------------------------------------------------
// Queries, returns and caches a shader uniform
//-------------------------------------------------------------------------------
IvUniform*
IvShaderProgramDX11::GetUniform(char const* name)
{
    // Did we already query the uniform?
    if (mUniforms.find(name) != mUniforms.end())
    {
        return static_cast<IvUniform*>(mUniforms[name]);
    }

	IvConstantDesc desc;

	// check the vertex shader first
	IvConstantTableDX11* constantTable;
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
		return NULL;
	}

	IvUniformDX11* uniform = new IvUniformDX11(desc.mType, desc.mOffset, desc.mCount, constantTable, this);

    mUniforms[name] = uniform;

    return uniform;
}
