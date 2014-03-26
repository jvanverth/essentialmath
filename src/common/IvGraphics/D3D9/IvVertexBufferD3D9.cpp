//===============================================================================
// @ IvVertexBufferD3D9.cpp
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

#include "IvVertexBufferD3D9.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static IDirect3DVertexDeclaration9* sVertexDeclaration[kVertexFormatCount] = {0};

D3DVERTEXELEMENT9 sCPFormatElements[] =
{
  {0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
  {0, 4, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
  D3DDECL_END() // this macro is needed as the last item!
};

D3DVERTEXELEMENT9 sNPFormatElements[] =
{
  {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
  {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
  D3DDECL_END() // this macro is needed as the last item!
};

D3DVERTEXELEMENT9 sCNPFormatElements[] =
{
  {0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
  {0, 4, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
  {0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
  D3DDECL_END() // this macro is needed as the last item!
};

D3DVERTEXELEMENT9 sTCPFormatElements[] =
{
  {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
  {0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
  {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
  D3DDECL_END() // this macro is needed as the last item!
};

D3DVERTEXELEMENT9 sTNPFormatElements[] =
{
  {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
  {0, 8, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
  {0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
  D3DDECL_END() // this macro is needed as the last item!
};

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::IvVertexBufferD3D9()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvVertexBufferD3D9::IvVertexBufferD3D9() : IvVertexBuffer(), mBufferPtr(0), mDataPtr(0)
{
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::~IvVertexBufferD3D9()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvVertexBufferD3D9::~IvVertexBufferD3D9()
{
    Destroy();
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::Create()
//-------------------------------------------------------------------------------
// Create the resources for the vertex buffer
//-------------------------------------------------------------------------------
bool
IvVertexBufferD3D9::Create( IvVertexFormat format, unsigned int numVertices, IDirect3DDevice9* device )
{
	if( FAILED( device->CreateVertexBuffer( numVertices*kIvVFSize[format],
			D3DUSAGE_WRITEONLY, 0/*format*/, D3DPOOL_MANAGED, &mBufferPtr, NULL ) ) )
	{
		mBufferPtr = 0;
		return false;
	}

	if ( sVertexDeclaration[format] == 0 )
	{
		switch (format)
		{
		case kCPFormat:
			device->CreateVertexDeclaration( sCPFormatElements, &sVertexDeclaration[format] );
			break;
		case kNPFormat:
			device->CreateVertexDeclaration( sNPFormatElements, &sVertexDeclaration[format] );
			break;
		case kCNPFormat:
			device->CreateVertexDeclaration( sCNPFormatElements, &sVertexDeclaration[format] );
			break;
		case kTCPFormat:
			device->CreateVertexDeclaration( sTCPFormatElements, &sVertexDeclaration[format] );
			break;
		case kTNPFormat:
			device->CreateVertexDeclaration( sTNPFormatElements, &sVertexDeclaration[format] );
			break;
		}
	}
    else
    {
        sVertexDeclaration[format]->AddRef();
    }

	mVertexFormat = format;
	mNumVertices = numVertices;

	return true;
}	

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::Destroy()
//-------------------------------------------------------------------------------
// Destroy the resources for the vertex buffer
//-------------------------------------------------------------------------------
void
IvVertexBufferD3D9::Destroy()
{
	if ( mBufferPtr )
	{
		// clear the handle and any associated memory
		mBufferPtr->Release();
		mBufferPtr = 0;
	}

    if ( sVertexDeclaration[mVertexFormat] )
    {
        if ( sVertexDeclaration[mVertexFormat]->Release() == 0 )
        {
            sVertexDeclaration[mVertexFormat] = 0;
        }
    }

    mNumVertices = 0;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::MakeActive()
//-------------------------------------------------------------------------------
// Make this the active vertex buffer
//-------------------------------------------------------------------------------
bool
IvVertexBufferD3D9::MakeActive( IDirect3DDevice9* device )
{
    if ( mBufferPtr == 0 || mNumVertices == 0 )
        return false;
    
	device->SetStreamSource(0, mBufferPtr, 0, kIvVFSize[mVertexFormat] );
	device->SetVertexDeclaration( sVertexDeclaration[mVertexFormat] );
    
    return true;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::BeginLoadData()
//-------------------------------------------------------------------------------
// Lock down the buffer and start loading
// Returns pointer to client side data area
//-------------------------------------------------------------------------------
void *
IvVertexBufferD3D9::BeginLoadData()
{
	// already loading data...
	if ( mDataPtr )
		return 0;
	if(FAILED(mBufferPtr->Lock(0, mNumVertices*kIvVFSize[mVertexFormat], &mDataPtr, 0 ) ) ) 
		return 0;

	// patch colors for engine order, if needed
	char* colorPtr;
	unsigned int stride = kIvVFSize[mVertexFormat];
	switch (mVertexFormat)
	{
        default:
            colorPtr = 0;
            break;
        case kCPFormat:
            colorPtr = reinterpret_cast<char*>(&static_cast<IvCPVertex*>(mDataPtr)->color);
            break;
        case kCNPFormat:
            colorPtr = reinterpret_cast<char*>(&static_cast<IvCNPVertex*>(mDataPtr)->color);
            break;
        case kTCPFormat:
            colorPtr = reinterpret_cast<char*>(&static_cast<IvTCPVertex*>(mDataPtr)->color);
            break;
	}
    
	if ( colorPtr )
	{
		for ( unsigned int i = 0; i < mNumVertices; ++i, colorPtr += stride )
		{
			unsigned char temp = colorPtr[0];
			colorPtr[0] = colorPtr[2];
			colorPtr[2] = temp;
		}
	}
    
    return mDataPtr;
}

//-------------------------------------------------------------------------------
// @ IvVertexBufferD3D9::EndLoadData()
//-------------------------------------------------------------------------------
// Unlock the buffer, we're done loading
// Returns true if all went well
//-------------------------------------------------------------------------------
bool
IvVertexBufferD3D9::EndLoadData()
{
	// patch up colors for D3D order, if needed
	char* colorPtr;
	unsigned int stride = kIvVFSize[mVertexFormat];
	switch (mVertexFormat)
	{
	default:
		colorPtr = 0;
		break;
	case kCPFormat:
		colorPtr = reinterpret_cast<char*>(&static_cast<IvCPVertex*>(mDataPtr)->color);
		break;
	case kCNPFormat:
		colorPtr = reinterpret_cast<char*>(&static_cast<IvCNPVertex*>(mDataPtr)->color);
		break;
	case kTCPFormat:
		colorPtr = reinterpret_cast<char*>(&static_cast<IvTCPVertex*>(mDataPtr)->color);
		break;
	}

	if ( colorPtr )
	{
		for ( unsigned int i = 0; i < mNumVertices; ++i, colorPtr += stride )
		{
			unsigned char temp = colorPtr[0];
			colorPtr[0] = colorPtr[2];
			colorPtr[2] = temp;
		}
	}

	// unlock the pointer and return
    mDataPtr = 0;
	if(FAILED(mBufferPtr->Unlock()))
		return false;
	else
		return true;
}

