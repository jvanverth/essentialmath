//===============================================================================
// @ IvIndexedGeometry.cpp
// ------------------------------------------------------------------------------
// Abstract scene graph object that supports rendering a geometry
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvIndexedGeometry.h"
#include <IvRenderer.h>
#include <IvRendererHelp.h>
#include <IvVector3.h>
#include <IvMatrix44.h>
#include <IvResourceManager.h>
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvIndexedGeometry::IvIndexedGeometry()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
IvIndexedGeometry::IvIndexedGeometry() : IvGeometry()
{
    mVertices = 0;
    mIndices = 0;

}  // End of IvIndexedGeometry::IvIndexedGeometry


//-------------------------------------------------------------------------------
// @ IvIndexedGeometry::~IvIndexedGeometry()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
IvIndexedGeometry::~IvIndexedGeometry()
{
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
	mVertices = 0;
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
    mIndices = 0;

}  // End of IvIndexedGeometry::~IvIndexedGeometry


//-------------------------------------------------------------------------------
// @ IvIndexedGeometry::CreateFromStream()
//-------------------------------------------------------------------------------
//
// Creates an IvIndexedGeometry object from a file of the following format
//
// VertexCount
//
// Vertex positions X Y Z for each vertex
// ...
//
// Vertex normals X Y Z for each vertex
// ...
// 
// Vertex colors R G B for each vertex
// ...
//
// Triangle index count (count of indices, not triangles, so this should be 3x
// the triangle count
//
// Triangle indices 
// ...
//
//-------------------------------------------------------------------------------
IvIndexedGeometry* IvIndexedGeometry::CreateFromStream(IvReader& in)
{
    IvIndexedGeometry* geom = new IvIndexedGeometry;
    IvCNPVertex* dataPtr = 0;  
    IvVector3* tempPosition = 0;
    UInt32* indexPtr = 0;

    // get number of vertices
    UInt32 numVerts;
    in >> numVerts;
    if ( !in.good() )
        goto error_exit;

    // read verts
    geom->mVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCNPFormat, numVerts,
                                                                                      NULL, kDefaultUsage);
    dataPtr = (IvCNPVertex*) geom->mVertices->BeginLoadData();
    tempPosition = new IvVector3[numVerts];	// save for capsule creation
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float x, y, z;
        in >> x >> y >> z;
        tempPosition[i].Set(x,y,z);
        dataPtr[i].position = tempPosition[i];
        if (!in.good() )
        {
            goto error_exit;
        }
    }

    // read normals
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float x, y, z;
        in >> x >> y >> z;
	dataPtr[i].normal.Set(x,y,z);
        if (!in.good() )
            goto error_exit;
    }

    // read colors
    for ( UInt32 i = 0; i < numVerts; ++i )
    {
        float r, g, b;
        in >> r >> g >> b;
        dataPtr[i].color.mRed = UChar8(r*255);
        dataPtr[i].color.mGreen = UChar8(g*255);
        dataPtr[i].color.mBlue = UChar8(b*255);
        dataPtr[i].color.mAlpha = 255;
        if (!in.good() )
            goto error_exit;
    }

    if (!geom->mVertices->EndLoadData())
    {
        goto error_exit;
    }
    dataPtr = 0;

    // get number of indices
    UInt32 numIndices;
    in >> numIndices;
    if ( !in.good() )
        goto error_exit;

    // read indices
    geom->mIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numIndices,
                                                                                    NULL, kDefaultUsage);
    indexPtr = static_cast<UInt32*>(geom->mIndices->BeginLoadData());
    for ( UInt32 i = 0; i < numIndices; ++i )
    {
        in >> indexPtr[i];
        if (!in.good() )
        {
            goto error_exit;
        }
    }
    if (!geom->mIndices->EndLoadData())
    {
        goto error_exit;
    }
    indexPtr = 0;
	
    // initialize the model-space capsule to the vertex data
    geom->mModelCapsule.Set( tempPosition, numVerts );
    delete [] tempPosition;

    return geom;

error_exit:
    // error cleanup case
    delete [] tempPosition;

    // delete the partially-created object
    delete geom;

    if ( dataPtr )
        (void) geom->mVertices->EndLoadData();
    if ( indexPtr )
        (void) geom->mIndices->EndLoadData();


    return 0;
}  // End of IvIndexedGeometry::CreateFromStream


//-------------------------------------------------------------------------------
// @ IvIndexedGeometry::Render()
//-------------------------------------------------------------------------------
//
// Draws the indexed geometry object using vertex arrays
//
//-------------------------------------------------------------------------------
void IvIndexedGeometry::Render()
{
    IvGeometry::Render();

    IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices, mIndices);

    if (gDisplayLeafBounds)
    {
		IvRenderer::mRenderer->SetFillMode(kWireframeFill);

        IvSetWorldIdentity();
        IvDrawCapsule(mWorldCapsule.GetSegment(), mWorldCapsule.GetRadius(), kOrange);
		IvRenderer::mRenderer->SetFillMode(kSolidFill);
    }
       
    if (gDisplayHierarchyBounds)
    {
		IvRenderer::mRenderer->SetFillMode(kWireframeFill);
        IvMatrix44 ident;
        ident.Identity();
        ident(0,3) = mWorldSphere.GetCenter().GetX();
        ident(1,3) = mWorldSphere.GetCenter().GetY();
        ident(2,3) = mWorldSphere.GetCenter().GetZ();

        IvSetWorldMatrix(ident);
        IvDrawSphere(mWorldSphere.GetRadius(), kYellow);

		IvRenderer::mRenderer->SetFillMode(kSolidFill);
    }

}  // End of IvIndexedGeometry::Render

