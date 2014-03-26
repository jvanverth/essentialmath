//===============================================================================
// @ IvIndexBufferD3D9.cpp
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

#include "IvIndexBufferD3D9.h"
#include "IvTypes.h"
#include <stdio.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::IvIndexBufferD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvIndexBufferD3D9::IvIndexBufferD3D9() : IvIndexBuffer(), mBufferPtr(0)
{
}	// End of IvIndexBufferD3D9::IvIndexBufferD3D9()

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::~IvIndexBufferD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvIndexBufferD3D9::~IvIndexBufferD3D9()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::Create()
//-------------------------------------------------------------------------------
// Create the platform-specific data
//-------------------------------------------------------------------------------
bool
IvIndexBufferD3D9::Create( unsigned int numIndices, IDirect3DDevice9* device )
{
	if( FAILED( device->CreateIndexBuffer( numIndices*sizeof(UInt32),
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &mBufferPtr, NULL ) ) )
	{
		mBufferPtr = 0;
		return false;
	}

	mNumIndices = numIndices;

	return true;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::Destroy()
//-------------------------------------------------------------------------------
// Destroy the platform-specific data
//-------------------------------------------------------------------------------
void
IvIndexBufferD3D9::Destroy()
{
	if (mBufferPtr)
	{
		// clear the handle and any associated memory
		mBufferPtr->Release();
		mBufferPtr = 0;
	}
    
    mNumIndices = 0;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active index buffer
//-------------------------------------------------------------------------------
bool
IvIndexBufferD3D9::MakeActive( IDirect3DDevice9* device )
{
    if ( mBufferPtr == 0 || mNumIndices == 0 )
        return false;
    
    device->SetIndices( mBufferPtr );
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::BeginLoadData()
//-------------------------------------------------------------------------------
// Lock down the buffer and start loading
// Returns pointer to client side data area
//-------------------------------------------------------------------------------
void *
IvIndexBufferD3D9::BeginLoadData()
{
	void* dataPtr = 0;
	if(FAILED(mBufferPtr->Lock(0, mNumIndices*sizeof(unsigned short), &dataPtr, 0 ) ) ) 
		return 0;

    return dataPtr;
}

//-------------------------------------------------------------------------------
// @ IvIndexBufferD3D9::EndLoadData()
//-------------------------------------------------------------------------------
// Unlock the buffer, we're done loading
// Returns true if all went well
//-------------------------------------------------------------------------------
bool
IvIndexBufferD3D9::EndLoadData()
{
    if(FAILED(mBufferPtr->Unlock()))
		return false;
	else
		return true;
}


