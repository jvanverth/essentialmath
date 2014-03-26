//===============================================================================
// @ IvShaderProgramD3D9.cpp
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

#include "IvShaderProgramD3D9.h"
#include "IvFragmentShaderD3D9.h"
#include "IvUniformD3D9.h"
#include "IvVertexShaderD3D9.h"

// probably don't belong here
#include "IvMatrix44.h"
#include "IvRendererD3D9.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D9::IvShaderProgramD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvShaderProgramD3D9::IvShaderProgramD3D9() : 
    IvShaderProgram(), 
    mVertexShaderPtr(0), 
    mFragmentShaderPtr(0),
    mVertexShaderConstants(0),
    mFragmentShaderConstants(0)
{
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D9::~IvShaderProgramD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvShaderProgramD3D9::~IvShaderProgramD3D9()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D9::Create()
//-------------------------------------------------------------------------------
// Create a shader program
//-------------------------------------------------------------------------------
bool
IvShaderProgramD3D9::Create( IvVertexShaderD3D9* vertexShaderPtr, IvFragmentShaderD3D9* fragmentShaderPtr )
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
// @ IvShaderProgramD3D9::Destroy()
//-------------------------------------------------------------------------------
// Clean up before destructor
//-------------------------------------------------------------------------------
void
IvShaderProgramD3D9::Destroy()
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

    std::map<std::string, IvUniformD3D9*>::iterator iter = mUniforms.begin();
    while (iter != mUniforms.end())
    {
        delete iter->second;
        iter++;
    }
    mUniforms.clear();
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D9::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active program
//-------------------------------------------------------------------------------
bool
IvShaderProgramD3D9::MakeActive( IDirect3DDevice9* device )
{
    if ( mVertexShaderPtr == 0 || mFragmentShaderPtr == 0 )
        return false;
	
	device->SetVertexShader( mVertexShaderPtr );
	device->SetPixelShader( mFragmentShaderPtr );

    std::map<std::string, IvUniformD3D9*>::iterator iter = mUniforms.begin();
   
    while (iter != mUniforms.end())
    {
        iter->second->Update();
        
        iter++;
    }

    return true;
}

//-------------------------------------------------------------------------------
// @ IvShaderProgramD3D9::GetUniform()
//-------------------------------------------------------------------------------
// Queries, returns and caches a shader uniform
//-------------------------------------------------------------------------------
IvUniform*
IvShaderProgramD3D9::GetUniform(char const* name)
{
    // Did we already query the uniform?
    if (mUniforms.find(name) != mUniforms.end())
    {
        return static_cast<IvUniform*>(mUniforms[name]);
    }

    // check the vertex shader first
    LPD3DXCONSTANTTABLE constantTable = mVertexShaderConstants;
    D3DXHANDLE cHandle = constantTable->GetConstantByName(NULL, name);

    // check the fragment/pixel shader last
    if (!cHandle)
    {
        constantTable = mFragmentShaderConstants;
        cHandle = constantTable->GetConstantByName(NULL, name);
    }
       
    if (!cHandle)
    {
        return NULL;
    }

    // get a constant description so we can tell what this is
    D3DXCONSTANT_DESC constantDesc;
    unsigned int descCount = 1;
    
    if ( D3D_OK != constantTable->GetConstantDesc( cHandle, &constantDesc, &descCount ) )
        return NULL;

    // set our local description
    unsigned int count = constantDesc.RegisterCount;
    IvUniformType ivType;
    switch ( constantDesc.Class )
    {
    case D3DXPC_SCALAR:
        if ( constantDesc.Type == D3DXPT_FLOAT )
            ivType = kFloatUniform;
        else
            return NULL;
        break;
    case D3DXPC_VECTOR:
        if ( constantDesc.Type == D3DXPT_FLOAT 
            && constantDesc.Columns == 4 )
            ivType = kFloat4Uniform;
        else
            return NULL;
        break;
    case D3DXPC_MATRIX_ROWS: 
    case D3DXPC_MATRIX_COLUMNS: 
        if ( constantDesc.Type == D3DXPT_FLOAT 
            && constantDesc.Rows == 4
            && constantDesc.Columns == 4 )
            ivType = kFloatMatrix44Uniform;
        else
            return NULL;
        break;
    case D3DXPC_OBJECT:
        if ( constantDesc.Type == D3DXPT_SAMPLER2D )
            ivType = kTextureUniform;
        else
            return NULL;
        break;
    default:
        return NULL;
    }

    IvUniformD3D9* uniform = new IvUniformD3D9(ivType, count, cHandle, constantTable, this);

    mUniforms[name] = uniform;

    return uniform;
}
