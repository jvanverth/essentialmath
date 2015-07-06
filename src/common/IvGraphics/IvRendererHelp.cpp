//===============================================================================
// @ IvRendererHelp.cpp
// 
// Helper routines to set up basic rendering functionality
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvResourceManager.h>
#include "IvRendererHelp.h"
#include <IvVector3.h>
#include <IvVector4.h>
#include <IvMatrix44.h>
#include <IvMatrix33.h>
#include <IvLineSegment3.h>
#include <IvMath.h>
#include <IvQuat.h>
#include <IvStackAllocator.h>
#include <IvVertexFormats.h>
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>
#include <IvFileReader.h>

#include "IvTeapot.h"

//-------------------------------------------------------------------------------
//-- Externals ------------------------------------------------------------------
//-------------------------------------------------------------------------------

IvColor kRed = {0xff, 0, 0, 0xff};
IvColor kGreen = {0, 0xff, 0, 0xff};
IvColor kBlue = {0, 0, 0xff, 0xff};
IvColor kMagenta = {0xff, 0, 0xff, 0xff};
IvColor kYellow = {0xff, 0xff, 0, 0xff};
IvColor kCyan = {0, 0xff, 0xff, 0xff};
IvColor kOrange = {0xff, 0x3f, 0x0f, 0xff};
IvColor kWhite = {0xff, 0xff, 0xff, 0xff};
IvColor kGrey = {0x3f, 0x3f, 0x3f, 0xff};
IvColor kBlack = {0, 0, 0, 0xff};

//-------------------------------------------------------------------------------
//-- Static Variables -----------------------------------------------------------
//-------------------------------------------------------------------------------

static IvVertexBuffer* axesVerts = 0;
static IvVertexBuffer* floorVerts = 0;
static IvIndexBuffer* floorIndices = 0;
static IvVertexBuffer* boxVerts = 0;
static IvIndexBuffer* boxIndices = 0;
static IvVertexBuffer* teapotVertexBuffer = 0;
static IvIndexBuffer* teapotIndexBuffer = 0;
static IvVertexBuffer* lineVertexBuffer = 0;
static IvVertexBuffer* sphereVertices = 0;
static IvIndexBuffer* sphereIndices = 0;
static IvVertexBuffer* capsuleTopVertices = 0;
static IvVertexBuffer* capsuleMidVertices = 0;
static IvVertexBuffer* capsuleBottomVertices = 0;
static IvIndexBuffer* capsuleTopIndices = 0;
static IvIndexBuffer* capsuleMidIndices = 0;
static IvIndexBuffer* capsuleBottomIndices = 0;

//-------------------------------------------------------------------------------
//-- Functions ------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvCleanUpRendererHelpData()
//-------------------------------------------------------------------------------
// Remove any vertex and index data still hanging around
//-------------------------------------------------------------------------------
void 
IvCleanUpRendererHelpData()
{
    if ( axesVerts )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(axesVerts);
        axesVerts = 0;
    }

    if ( floorVerts )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(floorVerts);
        floorVerts = 0;
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(floorIndices);
        floorIndices = 0;
    }

    if ( boxVerts )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(boxVerts);
        boxVerts = 0;
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(boxIndices);
        boxIndices = 0;
    }

    if ( teapotVertexBuffer )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotVertexBuffer);
        teapotVertexBuffer = 0;
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotIndexBuffer);
        teapotIndexBuffer = 0;
    }

    if ( lineVertexBuffer )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(lineVertexBuffer);
        lineVertexBuffer = 0;
    }

    if ( sphereVertices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(sphereVertices);
        sphereVertices = 0;
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(sphereIndices);
        sphereIndices = 0;
    }

	if ( capsuleTopVertices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleTopVertices);
        capsuleTopVertices = 0;
    }
	if ( capsuleMidVertices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleMidVertices);
        capsuleMidVertices = 0;
    }
	if ( capsuleBottomVertices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleBottomVertices);
        capsuleBottomVertices = 0;
    }

	if ( capsuleTopIndices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleTopIndices);
        capsuleTopIndices = 0;
    }
	if ( capsuleMidIndices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleMidIndices);
        capsuleMidIndices = 0;
    }
	if ( capsuleBottomIndices )
    {
	    IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleBottomIndices);
        capsuleBottomIndices = 0;
    }
}


//-------------------------------------------------------------------------------
// @ IvDrawAxes()
//-------------------------------------------------------------------------------
// Draw coordinate axes
//-------------------------------------------------------------------------------
void 
IvDrawAxes()
{
	// build data if not there
	if ( axesVerts == 0 )
	{
		size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
		IvCPVertex* dataPtr = (IvCPVertex*) IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kCPFormat] * 6);
        
        if (NULL == dataPtr)
        {
            return;
        }

		dataPtr[0].color.Set(255, 0, 0, 255);
		dataPtr[0].position.Set(0.0f, 0.0f, 0.0f);
		dataPtr[1].color.Set(255, 0, 0, 255);
		dataPtr[1].position.Set(2.5f, 0.0f, 0.0f);

		dataPtr[2].color.Set(0, 255, 0, 255);
		dataPtr[2].position.Set(0.0f, 0.0f, 0.0f);
		dataPtr[3].color.Set(0, 255, 0, 255);
		dataPtr[3].position.Set(0.0f, 2.5f, 0.0f);

		dataPtr[4].color.Set(0, 0, 255, 255);
		dataPtr[4].position.Set(0.0f, 0.0f, 0.0f);
		dataPtr[5].color.Set(0, 0, 255, 255);
		dataPtr[5].position.Set(0.0f, 0.0f, 2.5f);

		axesVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 6, dataPtr,
                                                                                    kImmutableUsage);

		IvStackAllocator::mScratchAllocator->Reset(currentOffset);
	}

    IvSetWorldIdentity();

	// clear to default shader
	IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
	IvRenderer::mRenderer->SetShaderProgram(0);

    // draw it
    IvRenderer::mRenderer->Draw(kLineListPrim, axesVerts);

	// restore original shader
	IvRenderer::mRenderer->SetShaderProgram(oldShader);

}   // End of IvDrawAxes()


//-------------------------------------------------------------------------------
// @ IvDrawFloor()
//-------------------------------------------------------------------------------
// Draw tiled floor at the origin
//-------------------------------------------------------------------------------
void 
IvDrawFloor()
{
	// build data if not there
	if ( floorVerts == 0 )
	{
		size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
		IvCPVertex* vertexPtr = (IvCPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kCPFormat] * 4 * 81); 
		UInt32* indexPtr = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32) * 6 * 81);
		int currentVertex = 0;
		int currentIndex = 0;
		for ( int i = -12; i <= 12; i += 3 )
		{
			for ( int j = -12; j <= 12; j += 3 )
			{
				IvColor color;
				if ( ( i & 0x1 ) == (j & 0x1 ))
					color.Set( 64, 64, 64, 255 );
				else
					color.Set( 192, 192, 192, 255 );
				vertexPtr[currentVertex].position.Set(0.0f + (float)i, 0.0f + (float)j, 0.0f);
				vertexPtr[currentVertex].color = color;
				vertexPtr[currentVertex+1].position.Set(3.0f + (float)i, 0.0f + (float)j, 0.0f);
				vertexPtr[currentVertex+1].color = color;
				vertexPtr[currentVertex+2].position.Set(0.0f + (float)i, 3.0f + (float)j, 0.0f);
				vertexPtr[currentVertex+2].color = color;
				vertexPtr[currentVertex+3].position.Set(3.0f + (float)i, 3.0f + (float)j, 0.0f);
				vertexPtr[currentVertex+3].color = color;
				indexPtr[currentIndex++] = currentVertex;
				indexPtr[currentIndex++] = currentVertex+1;
				indexPtr[currentIndex++] = currentVertex+2;
				indexPtr[currentIndex++] = currentVertex+2;
				indexPtr[currentIndex++] = currentVertex+1;
				indexPtr[currentIndex++] = currentVertex+3;
				currentVertex += 4;
			}
		}

		floorVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 4 * 81, vertexPtr,
                                                                                     kImmutableUsage);
		floorIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(6 * 81, indexPtr,
                                                                                      kImmutableUsage);

		if (!floorVerts || !floorIndices)
		{
			if (floorVerts)
			{
				IvRenderer::mRenderer->GetResourceManager()->Destroy(floorVerts);
				floorVerts = 0;
			}
			if (floorIndices)
			{
				IvRenderer::mRenderer->GetResourceManager()->Destroy(floorIndices);
				floorIndices = 0;
			}
		}

		IvStackAllocator::mScratchAllocator->Reset(currentOffset);
	}

    IvSetWorldIdentity();

	// clear to default shader
	IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
	IvRenderer::mRenderer->SetShaderProgram(0);

    // draw it
    IvRenderer::mRenderer->Draw(kTriangleListPrim, floorVerts, floorIndices);

	// restore original shader
	IvRenderer::mRenderer->SetShaderProgram(oldShader);

}   // End of IvDrawFloor()

//-------------------------------------------------------------------------------
// @ IvDrawCube()
//-------------------------------------------------------------------------------
// Draw unit cube around origin
//-------------------------------------------------------------------------------
void 
IvDrawCube( IvColor color )
{
    IvVector3 minima( -1.0f, -1.0f, -1.0f );
    IvVector3 maxima( 1.0f, 1.0f, 1.0f );
    IvDrawBox( minima, maxima, color );

}   // End of IvDrawCube()


//-------------------------------------------------------------------------------
// @ IvDrawBox()
//-------------------------------------------------------------------------------
// Draw box 
//-------------------------------------------------------------------------------
void 
IvDrawBox( const IvVector3& minima, const IvVector3& maxima, IvColor color )
{
	// load data if not there
	if ( boxVerts == 0 )
	{
		size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
		IvNPVertex* vertexPtr = (IvNPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kNPFormat] * 4 * 6);
		UInt32* indexPtr = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32) * 6 * 6);

		int currentVertex = 0;
		int currentIndex = 0;

	    // set geometry
		// top
		vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex].normal = IvVector3::zAxis;
		vertexPtr[currentVertex+1].position.Set(minima.GetX(), maxima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex+1].normal = IvVector3::zAxis;
		vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex+2].normal = IvVector3::zAxis;
		vertexPtr[currentVertex+3].position.Set(maxima.GetX(), minima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex+3].normal = IvVector3::zAxis;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+1;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+3;
		currentVertex += 4;

		// bottom
		vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), minima.GetZ());
		vertexPtr[currentVertex].normal = -IvVector3::zAxis;
		vertexPtr[currentVertex+1].position.Set(minima.GetX(), maxima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+1].normal = -IvVector3::zAxis;
		vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+2].normal = -IvVector3::zAxis;
		vertexPtr[currentVertex+3].position.Set(maxima.GetX(), minima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+3].normal = -IvVector3::zAxis;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+1;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+3;
		indexPtr[currentIndex++] = currentVertex+2;
		currentVertex += 4;

		// left
		vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex].normal = IvVector3::yAxis;
		vertexPtr[currentVertex+1].position.Set(maxima.GetX(), maxima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+1].normal = IvVector3::yAxis;
		vertexPtr[currentVertex+2].position.Set(minima.GetX(), maxima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+2].normal = IvVector3::yAxis;
		vertexPtr[currentVertex+3].position.Set(minima.GetX(), maxima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex+3].normal = IvVector3::yAxis;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+1;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+3;
		currentVertex += 4;    

		// right
		vertexPtr[currentVertex].position.Set(maxima.GetX(), minima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex].normal = -IvVector3::yAxis;
		vertexPtr[currentVertex+1].position.Set(maxima.GetX(), minima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+1].normal = -IvVector3::yAxis;
		vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+2].normal = -IvVector3::yAxis;
		vertexPtr[currentVertex+3].position.Set(minima.GetX(), minima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex+3].normal = -IvVector3::yAxis;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+1;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+3;
		indexPtr[currentIndex++] = currentVertex+2;
		currentVertex += 4;  

		// front
		vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex].normal = IvVector3::xAxis;
		vertexPtr[currentVertex+1].position.Set(maxima.GetX(), minima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex].normal = IvVector3::xAxis;
		vertexPtr[currentVertex+2].position.Set(maxima.GetX(), minima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+2].normal = IvVector3::xAxis;
		vertexPtr[currentVertex+3].position.Set(maxima.GetX(), maxima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+3].normal = IvVector3::xAxis;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+1;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+3;
		currentVertex += 4;  

		// back
		vertexPtr[currentVertex].position.Set(minima.GetX(), maxima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex].normal = -IvVector3::xAxis;
		vertexPtr[currentVertex+1].position.Set(minima.GetX(), minima.GetY(), maxima.GetZ());
		vertexPtr[currentVertex+1].normal = -IvVector3::xAxis;
		vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+2].normal = -IvVector3::xAxis;
		vertexPtr[currentVertex+3].position.Set(minima.GetX(), maxima.GetY(), minima.GetZ());
		vertexPtr[currentVertex+3].normal = -IvVector3::xAxis;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+2;
		indexPtr[currentIndex++] = currentVertex+1;
		indexPtr[currentIndex++] = currentVertex+0;
		indexPtr[currentIndex++] = currentVertex+3;
		indexPtr[currentIndex++] = currentVertex+2;
		currentVertex += 4;  

		boxVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kNPFormat, 4 * 6, vertexPtr,
                                                                                   kImmutableUsage);
		boxIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(6 * 6, indexPtr,
                                                                                    kImmutableUsage);

		if (!boxVerts || !boxIndices)
		{
			if (boxVerts)
			{
				IvRenderer::mRenderer->GetResourceManager()->Destroy(boxVerts);
				boxVerts = 0;
			}
			if (boxIndices)
			{
				IvRenderer::mRenderer->GetResourceManager()->Destroy(boxIndices);
				boxIndices = 0;
			}
		}
        
		IvStackAllocator::mScratchAllocator->Reset(currentOffset);
	}

	// clear to default shader
	IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
	IvRenderer::mRenderer->SetShaderProgram(0);

    // draw it
	IvShadeMode oldShadeMode = IvRenderer::mRenderer->GetShadeMode();
	IvRenderer::mRenderer->SetShadeMode( kFlatShaded );
	IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    IvRenderer::mRenderer->Draw(kTriangleListPrim, boxVerts, boxIndices);

	// restore original state
	IvRenderer::mRenderer->SetDefaultDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	IvRenderer::mRenderer->SetShaderProgram( oldShader );
	IvRenderer::mRenderer->SetShadeMode( oldShadeMode );

}   // End of IvDrawBox()

//-------------------------------------------------------------------------------
// @ IvDrawBox()
//-------------------------------------------------------------------------------
// Draw box
//-------------------------------------------------------------------------------
void
IvDrawBoxNoShader( const IvVector3& minima, const IvVector3& maxima, IvColor color )
{
    // load data if not there
    if ( boxVerts == 0 )
    {
        size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
        IvNPVertex* vertexPtr = (IvNPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kNPFormat] * 4 * 6);
        UInt32* indexPtr = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32) * 6 * 6);
        
        int currentVertex = 0;
        int currentIndex = 0;
        
        // set geometry
        // top
        vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex].normal = IvVector3::zAxis;
        vertexPtr[currentVertex+1].position.Set(minima.GetX(), maxima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex+1].normal = IvVector3::zAxis;
        vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex+2].normal = IvVector3::zAxis;
        vertexPtr[currentVertex+3].position.Set(maxima.GetX(), minima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex+3].normal = IvVector3::zAxis;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+1;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+3;
        currentVertex += 4;
        
        // bottom
        vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), minima.GetZ());
        vertexPtr[currentVertex].normal = -IvVector3::zAxis;
        vertexPtr[currentVertex+1].position.Set(minima.GetX(), maxima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+1].normal = -IvVector3::zAxis;
        vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+2].normal = -IvVector3::zAxis;
        vertexPtr[currentVertex+3].position.Set(maxima.GetX(), minima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+3].normal = -IvVector3::zAxis;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+1;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+3;
        indexPtr[currentIndex++] = currentVertex+2;
        currentVertex += 4;
        
        // left
        vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex].normal = IvVector3::yAxis;
        vertexPtr[currentVertex+1].position.Set(maxima.GetX(), maxima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+1].normal = IvVector3::yAxis;
        vertexPtr[currentVertex+2].position.Set(minima.GetX(), maxima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+2].normal = IvVector3::yAxis;
        vertexPtr[currentVertex+3].position.Set(minima.GetX(), maxima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex+3].normal = IvVector3::yAxis;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+1;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+3;
        currentVertex += 4;
        
        // right
        vertexPtr[currentVertex].position.Set(maxima.GetX(), minima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex].normal = -IvVector3::yAxis;
        vertexPtr[currentVertex+1].position.Set(maxima.GetX(), minima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+1].normal = -IvVector3::yAxis;
        vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+2].normal = -IvVector3::yAxis;
        vertexPtr[currentVertex+3].position.Set(minima.GetX(), minima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex+3].normal = -IvVector3::yAxis;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+1;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+3;
        indexPtr[currentIndex++] = currentVertex+2;
        currentVertex += 4;
        
        // front
        vertexPtr[currentVertex].position.Set(maxima.GetX(), maxima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex].normal = IvVector3::xAxis;
        vertexPtr[currentVertex+1].position.Set(maxima.GetX(), minima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex].normal = IvVector3::xAxis;
        vertexPtr[currentVertex+2].position.Set(maxima.GetX(), minima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+2].normal = IvVector3::xAxis;
        vertexPtr[currentVertex+3].position.Set(maxima.GetX(), maxima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+3].normal = IvVector3::xAxis;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+1;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+3;
        currentVertex += 4;
        
        // back
        vertexPtr[currentVertex].position.Set(minima.GetX(), maxima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex].normal = -IvVector3::xAxis;
        vertexPtr[currentVertex+1].position.Set(minima.GetX(), minima.GetY(), maxima.GetZ());
        vertexPtr[currentVertex+1].normal = -IvVector3::xAxis;
        vertexPtr[currentVertex+2].position.Set(minima.GetX(), minima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+2].normal = -IvVector3::xAxis;
        vertexPtr[currentVertex+3].position.Set(minima.GetX(), maxima.GetY(), minima.GetZ());
        vertexPtr[currentVertex+3].normal = -IvVector3::xAxis;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+2;
        indexPtr[currentIndex++] = currentVertex+1;
        indexPtr[currentIndex++] = currentVertex+0;
        indexPtr[currentIndex++] = currentVertex+3;
        indexPtr[currentIndex++] = currentVertex+2;
        currentVertex += 4;
        
        boxVerts = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kNPFormat, 4 * 6, vertexPtr,
                                                                                   kImmutableUsage);
        boxIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(6 * 6, indexPtr,
                                                                                    kImmutableUsage);
        
        if (!boxVerts || !boxIndices)
        {
            if (boxVerts)
            {
                IvRenderer::mRenderer->GetResourceManager()->Destroy(boxVerts);
                boxVerts = 0;
            }
            if (boxIndices)
            {
                IvRenderer::mRenderer->GetResourceManager()->Destroy(boxIndices);
                boxIndices = 0;
            }
        }
        
        IvStackAllocator::mScratchAllocator->Reset(currentOffset);
    }
    
//    // clear to default shader
//    IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
//    IvRenderer::mRenderer->SetShaderProgram(0);
//    
//    // draw it
//    IvShadeMode oldShadeMode = IvRenderer::mRenderer->GetShadeMode();
//    IvRenderer::mRenderer->SetShadeMode( kFlatShaded );
//    IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    IvRenderer::mRenderer->Draw(kTriangleListPrim, boxVerts, boxIndices);
    
//    // restore original state
//    IvRenderer::mRenderer->SetDefaultDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
//    IvRenderer::mRenderer->SetShaderProgram( oldShader );
//    IvRenderer::mRenderer->SetShadeMode( oldShadeMode );
    
}   // End of IvDrawBox()

//-------------------------------------------------------------------------------
// @ IvDrawOBB()
//-------------------------------------------------------------------------------
// Draw oriented box 
//-------------------------------------------------------------------------------
void 
IvDrawOBB( const IvVector3& extents, const IvVector3& center, const IvMatrix33& axes, IvColor color )
{
    IvVector3 minima = -extents;
    IvMatrix33 mat = ::Transpose(axes);
    IvMatrix44 transform( mat );
    transform(0,3) = center.GetX();
    transform(1,3) = center.GetY();
    transform(2,3) = center.GetZ();

    IvSetWorldMatrix( transform );
    IvDrawBox( minima, extents, color );

}   // End of IvDrawOBB()


//-------------------------------------------------------------------------------
// @ CreateSphere()
//-------------------------------------------------------------------------------
// Set up sphere vertices
//-------------------------------------------------------------------------------
static void
CreateSphere()
{
    // load data if not there
	if ( sphereVertices == 0 )
	{
		size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
        
		const int stacks = 16;
		const int slices = 24;
        const int numVerts = slices*(stacks-1)+2;
        const int numIndices = 6*slices*(stacks-1);
        
		IvNPVertex* vertexPtr = (IvNPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kNPFormat] * numVerts);
		UInt32* indexPtr = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32) * numIndices);
        
		int currentVertex = 0;
		int currentIndex = 0;
		
		// verts
		vertexPtr[currentVertex].normal.Set(0.0, 0.0f, -1.0f);
		vertexPtr[currentVertex].position.Set(0.0, 0.0f, -1.0f);
		currentVertex++;
		const float increment = kPI/(float)stacks;
		const float thetaIncrement = kTwoPI/(float)slices;
        
		for (UInt32 latitude = 1; latitude < stacks; ++latitude)
		{
			for (UInt32 longitude = 0; longitude < slices; ++longitude)
			{
				float phi = -kHalfPI + float(latitude)*increment;
				float theta = float(longitude)*thetaIncrement;
                
				float sinTheta, cosTheta;
				IvSinCos(theta, sinTheta, cosTheta);
                
				float sinPhi, cosPhi;
				IvSinCos(phi, sinPhi, cosPhi);
                
				vertexPtr[currentVertex].normal.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);
				vertexPtr[currentVertex].position.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);
                
				if ( longitude > 0 )
				{
					if ( currentVertex >= 1+slices )
					{
						indexPtr[currentIndex++] = currentVertex+0;
						indexPtr[currentIndex++] = currentVertex-1;
						indexPtr[currentIndex++] = currentVertex-slices-1;
						indexPtr[currentIndex++] = currentVertex+0;
						indexPtr[currentIndex++] = currentVertex-slices-1;
						indexPtr[currentIndex++] = currentVertex-slices;
					}
					else
					{
						indexPtr[currentIndex++] = 0;
						indexPtr[currentIndex++] = currentVertex;
						indexPtr[currentIndex++] = currentVertex-1;
					}
				}
				currentVertex++;
			}
			if ( currentVertex >= 2*slices )
			{
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-1;
				indexPtr[currentIndex++] = currentVertex-slices-1;
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-slices-1;
				indexPtr[currentIndex++] = currentVertex-2*slices;
			}
			else
			{
				indexPtr[currentIndex++] = 0;
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-1;
			}
		}
        
		vertexPtr[currentVertex].normal.Set(0.0, 0.0f, 1.0f);
		vertexPtr[currentVertex].position.Set(0.0, 0.0f, 1.0f);
        
		for (UInt32 longitude = 0; longitude < slices-1; ++longitude)
		{
			indexPtr[currentIndex++] = currentVertex;
			indexPtr[currentIndex++] = currentVertex-longitude-2;
			indexPtr[currentIndex++] = currentVertex-longitude-1;
		}
		indexPtr[currentIndex++] = currentVertex;
		indexPtr[currentIndex++] = currentVertex-1;
		indexPtr[currentIndex++] = currentVertex-slices;
        
		sphereVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kNPFormat, numVerts, vertexPtr,
                                                                                         kImmutableUsage);
		sphereIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numIndices, indexPtr,
                                                                                       kImmutableUsage);
        
		if (!sphereVertices || !sphereIndices)
		{
            if (sphereVertices)
            {
                IvRenderer::mRenderer->GetResourceManager()->Destroy(sphereVertices);
                sphereVertices = 0;
            }
            if (sphereIndices)
            {
                IvRenderer::mRenderer->GetResourceManager()->Destroy(sphereIndices);
                sphereIndices = 0;
            }
		}
        
        IvStackAllocator::mScratchAllocator->Reset(currentOffset);
	}
}

//-------------------------------------------------------------------------------
// @ IvDrawSphere()
//-------------------------------------------------------------------------------
// Draw sphere
//-------------------------------------------------------------------------------
void 
IvDrawSphere( float radius, IvColor color )
{
	// clear to default shader
	IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
	IvRenderer::mRenderer->SetShaderProgram(0);

    // create matrix to scale to radius
    IvMatrix44 transform;
	transform.Scaling(IvVector3(radius,radius,radius));

    // concatenate with the current matrix
    IvMatrix44 currentMat = IvRenderer::mRenderer->GetWorldMatrix();

    // multiply by transform
    IvRenderer::mRenderer->SetWorldMatrix(currentMat * transform);
        
    // draw it
	IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    IvDrawUnitSphere();

	// restore original state
	IvRenderer::mRenderer->SetDefaultDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	IvRenderer::mRenderer->SetShaderProgram( oldShader );
    IvRenderer::mRenderer->SetWorldMatrix(currentMat);

}   // End of IvDrawSphere()

//-------------------------------------------------------------------------------
// @ IvDrawUnitSphere()
//-------------------------------------------------------------------------------
// Draw sphere
//-------------------------------------------------------------------------------
void
IvDrawUnitSphere()
{
    CreateSphere();
    
   // draw it
    IvRenderer::mRenderer->Draw(kTriangleListPrim, sphereVertices, sphereIndices);
    
}   // End of IvDrawUnitSphere()


//-------------------------------------------------------------------------------
// @ IvDrawCapsule()
//-------------------------------------------------------------------------------
// Draw capsule
//-------------------------------------------------------------------------------
void 
IvDrawCapsule( const IvLineSegment3& segment, float radius, IvColor color )
{
	// load data if not there
	const int stacks = 16;
	const int slices = 24;
	if ( capsuleBottomVertices == 0 )
	{
		capsuleBottomVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
                                            kNPFormat, slices*stacks/2+1, NULL, kDefaultUsage);
		IvNPVertex* vertexPtr = (IvNPVertex*) capsuleBottomVertices->BeginLoadData();
		capsuleBottomIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(
                                            3*slices*stacks, NULL, kDefaultUsage);
		UInt32* indexPtr = (UInt32*) capsuleBottomIndices->BeginLoadData();
		int currentVertex = 0;
		int currentIndex = 0;
		
		// verts
		vertexPtr[currentVertex].normal.Set(0.0, 0.0f, -1.0f);
		vertexPtr[currentVertex].position.Set(0.0, 0.0f, -1.0f);
		currentVertex++;
		const float increment = kPI/(float)stacks;
		const float thetaIncrement = kTwoPI/(float)slices;

		for (UInt32 latitude = 1; latitude <= stacks/2; ++latitude)
		{
			for (UInt32 longitude = 0; longitude < slices; ++longitude)
			{
				float phi = -kHalfPI + float(latitude)*increment;
				float theta = float(longitude)*thetaIncrement;

				float sinTheta, cosTheta;
				IvSinCos(theta, sinTheta, cosTheta);

				float sinPhi, cosPhi;
				IvSinCos(phi, sinPhi, cosPhi);

				vertexPtr[currentVertex].normal.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);
				vertexPtr[currentVertex].position.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);

				if ( longitude > 0 )
				{
					if ( currentVertex >= 1+slices )
					{
						indexPtr[currentIndex++] = currentVertex+0;
						indexPtr[currentIndex++] = currentVertex-1;
						indexPtr[currentIndex++] = currentVertex-slices-1;
						indexPtr[currentIndex++] = currentVertex+0;
						indexPtr[currentIndex++] = currentVertex-slices-1;
						indexPtr[currentIndex++] = currentVertex-slices;
					}
					else
					{
						indexPtr[currentIndex++] = 0;
						indexPtr[currentIndex++] = currentVertex;
						indexPtr[currentIndex++] = currentVertex-1;
					}
				}
				currentVertex++;
			}
			if ( currentVertex >= 2*slices )
			{
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-1;
				indexPtr[currentIndex++] = currentVertex-slices-1;
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-slices-1;
				indexPtr[currentIndex++] = currentVertex-2*slices;
			}
			else
			{
				indexPtr[currentIndex++] = 0;
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-1;
			}
		}

		if (!capsuleBottomVertices->EndLoadData() || !capsuleBottomIndices->EndLoadData())
		{
			IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleBottomVertices);
			capsuleBottomVertices = 0;
			IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleBottomIndices);
			capsuleBottomIndices = 0;
		}    
	}

	if ( capsuleTopVertices == 0 )
	{
		capsuleTopVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
                                                  kNPFormat, slices*stacks/2+1, NULL, kDefaultUsage);
		IvNPVertex* vertexPtr = (IvNPVertex*) capsuleTopVertices->BeginLoadData();
		capsuleTopIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(
                                                  3*slices*stacks, NULL, kDefaultUsage);
		UInt32* indexPtr = (UInt32*) capsuleTopIndices->BeginLoadData();
		int currentVertex = 0;
		int currentIndex = 0;
		
		// verts
		const float increment = kPI/(float)stacks;
		const float thetaIncrement = kTwoPI/(float)slices;

		for (UInt32 latitude = stacks/2; latitude < stacks; ++latitude)
		{
			for (UInt32 longitude = 0; longitude < slices; ++longitude)
			{
				float phi = -kHalfPI + float(latitude)*increment;
				float theta = float(longitude)*thetaIncrement;

				float sinTheta, cosTheta;
				IvSinCos(theta, sinTheta, cosTheta);

				float sinPhi, cosPhi;
				IvSinCos(phi, sinPhi, cosPhi);

				vertexPtr[currentVertex].normal.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);
				vertexPtr[currentVertex].position.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);

				if ( longitude > 0 )
				{
					if ( currentVertex >= 1+slices )
					{
						indexPtr[currentIndex++] = currentVertex+0;
						indexPtr[currentIndex++] = currentVertex-1;
						indexPtr[currentIndex++] = currentVertex-slices-1;
						indexPtr[currentIndex++] = currentVertex+0;
						indexPtr[currentIndex++] = currentVertex-slices-1;
						indexPtr[currentIndex++] = currentVertex-slices;
					}
				}
				currentVertex++;
			}
			if ( currentVertex >= 2*slices )
			{
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-1;
				indexPtr[currentIndex++] = currentVertex-slices-1;
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-slices-1;
				indexPtr[currentIndex++] = currentVertex-2*slices;
			}
			else
			{
				indexPtr[currentIndex++] = 0;
				indexPtr[currentIndex++] = currentVertex-slices;
				indexPtr[currentIndex++] = currentVertex-1;
			}
		}

		vertexPtr[currentVertex].normal.Set(0.0, 0.0f, 1.0f);
		vertexPtr[currentVertex].position.Set(0.0, 0.0f, 1.0f);

		for (UInt32 longitude = 0; longitude < slices-1; ++longitude)
		{
			indexPtr[currentIndex++] = currentVertex;
			indexPtr[currentIndex++] = currentVertex-longitude-2;
			indexPtr[currentIndex++] = currentVertex-longitude-1;
		}
		indexPtr[currentIndex++] = currentVertex;
		indexPtr[currentIndex++] = currentVertex-1;
		indexPtr[currentIndex++] = currentVertex-slices;

		if (!capsuleTopVertices->EndLoadData() || !capsuleTopIndices->EndLoadData())
		{
			IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleTopVertices);
			capsuleTopVertices = 0;
			IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleTopIndices);
			capsuleTopIndices = 0;
		}    
	}
    
	if ( capsuleMidVertices == 0 )
	{
		capsuleMidVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(
                                                            kNPFormat, slices*2, NULL, kDefaultUsage);
		IvNPVertex* vertexPtr = (IvNPVertex*) capsuleMidVertices->BeginLoadData();
		capsuleMidIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(
                                                            6*slices, NULL, kDefaultUsage);
		UInt32* indexPtr = (UInt32*) capsuleMidIndices->BeginLoadData();
		int currentVertex = 0;
		int currentIndex = 0;
		
		// verts
		const float thetaIncrement = kTwoPI/(float)slices;

        for (UInt32 z = 0; z <= 1; ++z)
        {
		    for (UInt32 longitude = 0; longitude < slices; ++longitude)
		    {
			    float theta = float(longitude)*thetaIncrement;

			    float sinTheta, cosTheta;
			    IvSinCos(theta, sinTheta, cosTheta);

			    vertexPtr[currentVertex].normal.Set(cosTheta, sinTheta, 0.0f);
			    vertexPtr[currentVertex].position.Set(cosTheta, sinTheta, (float)z);

			    if ( longitude > 0 )
			    {
				    if ( currentVertex >= slices )
				    {
					    indexPtr[currentIndex++] = currentVertex+0;
					    indexPtr[currentIndex++] = currentVertex-1;
					    indexPtr[currentIndex++] = currentVertex-slices-1;
					    indexPtr[currentIndex++] = currentVertex+0;
					    indexPtr[currentIndex++] = currentVertex-slices-1;
					    indexPtr[currentIndex++] = currentVertex-slices;
				    }
			    }
			    currentVertex++;
		    }
        }
		indexPtr[currentIndex++] = currentVertex-slices;
		indexPtr[currentIndex++] = currentVertex-1;
		indexPtr[currentIndex++] = currentVertex-slices-1;
		indexPtr[currentIndex++] = currentVertex-slices;
		indexPtr[currentIndex++] = currentVertex-slices-1;
		indexPtr[currentIndex++] = currentVertex-2*slices;

		if (!capsuleMidVertices->EndLoadData() || !capsuleMidIndices->EndLoadData())
		{
			IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleMidVertices);
			capsuleMidVertices = 0;
			IvRenderer::mRenderer->GetResourceManager()->Destroy(capsuleMidIndices);
			capsuleMidIndices = 0;
		}    
	}
    
    // this is kind of a cheat
    // we are going to draw this in a local space oriented along the z axis
    // so first we need to calculate a transform to convert from our space to 
    // the orientation and position of the actual capsule
    
    // get direction of segment axis
    IvVector3 direction = segment.GetEndpoint1() - segment.GetEndpoint0();
    float length = direction.Length();

    // if spherical capsule, just draw sphere
    if ( ::IsZero( length ) )
    {
        IvMatrix44 transform;
        transform(0,3) = segment.GetEndpoint0().GetX();
        transform(1,3) = segment.GetEndpoint0().GetY();
        transform(2,3) = segment.GetEndpoint0().GetZ();

        // concatenate with the current matrix
        IvMatrix44 currentMat = IvRenderer::mRenderer->GetWorldMatrix();

        // multiply by transform
        IvRenderer::mRenderer->SetWorldMatrix(currentMat * transform);

        ::IvDrawSphere( radius );
        
        // restore current matrix
        IvRenderer::mRenderer->SetWorldMatrix(currentMat);

        return;
    }

    // rotate z axis to segment axis
    direction.Normalize();
    IvQuat quat( IvVector3::zAxis, direction );

    // create matrix to align with axis, then translate the origin to endpoint0
    IvMatrix44 transform(quat);
    transform(0,0) *= radius;
    transform(1,0) *= radius;
    transform(2,0) *= radius;
    transform(0,1) *= radius;
    transform(1,1) *= radius;
    transform(2,1) *= radius;
    transform(0,2) *= radius;
    transform(1,2) *= radius;
    transform(2,2) *= radius;
    transform(0,3) = segment.GetEndpoint0().GetX();
    transform(1,3) = segment.GetEndpoint0().GetY();
    transform(2,3) = segment.GetEndpoint0().GetZ();

    // concatenate with the current matrix
    IvMatrix44 currentMat = IvRenderer::mRenderer->GetWorldMatrix();

    // multiply by transform
    IvRenderer::mRenderer->SetWorldMatrix(currentMat * transform);

    // clear to default shader
	IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
	IvRenderer::mRenderer->SetShaderProgram(0);

    // draw it
	IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    IvRenderer::mRenderer->Draw(kTriangleListPrim, capsuleBottomVertices, capsuleBottomIndices);

    IvMatrix44 xlate;
    xlate.Translation(IvVector3(0.0f,0.0f,length/radius));
    IvMatrix44 rotate;
    rotate.RotationX( kPI );
    IvRenderer::mRenderer->SetWorldMatrix(currentMat * transform * xlate );
    IvRenderer::mRenderer->Draw(kTriangleListPrim, capsuleTopVertices, capsuleTopIndices);

    transform(0,2) *= length/radius;
    transform(1,2) *= length/radius;
    transform(2,2) *= length/radius;
    IvRenderer::mRenderer->SetWorldMatrix(currentMat * transform);
    IvRenderer::mRenderer->Draw(kTriangleListPrim, capsuleMidVertices, capsuleMidIndices);

    // restore original state
	IvRenderer::mRenderer->SetDefaultDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	IvRenderer::mRenderer->SetShaderProgram(oldShader);

}   // End of IvDrawCapsule()


//-------------------------------------------------------------------------------
// @ IvDrawLine()
//-------------------------------------------------------------------------------
// Draw line segment from one 3D location to another
//
// Locking and unlocking this buffer, and using such a small buffer are 
// inefficient.  However, in this case the simplicity of interface wins.
//-------------------------------------------------------------------------------
void 
IvDrawLine( const IvVector3& from, const IvVector3& to, IvColor color )
{
	// create vertex buffer if not there
	if ( lineVertexBuffer == 0 )
	{
		lineVertexBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 2,
                                                                                           NULL, kDynamicUsage);
	}
    
	IvCPVertex* dataPtr = (IvCPVertex*) lineVertexBuffer->BeginLoadData();
    dataPtr[0].position = from;
    dataPtr[0].color = color;
    dataPtr[1].position = to;
    dataPtr[1].color = color;
	(void) lineVertexBuffer->EndLoadData();

    // clear to default shader
	IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
	IvRenderer::mRenderer->SetShaderProgram(0);

    // draw it
    IvRenderer::mRenderer->Draw(kLineListPrim, lineVertexBuffer);

    // restore data
	IvRenderer::mRenderer->SetShaderProgram(oldShader);

}   // End of IvDrawLine()


//-------------------------------------------------------------------------------
// @ IvDrawTeapot()
//-------------------------------------------------------------------------------
// Draw teapot, oriented with z pointing up
//-------------------------------------------------------------------------------
void 
IvDrawTeapot( IvColor color, bool useDefaultShader )
{
	// load data if not there
	if ( teapotVertexBuffer == 0 )
	{
        std::vector<IvTNPVertex> vertices;
        std::vector<unsigned int> indices;

        BuildTeapot( vertices, indices );

        unsigned int numTeapotVerts = vertices.size();
		size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
		IvNPVertex* dataPtr = (IvNPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kNPFormat] * numTeapotVerts);        
        for ( unsigned int i = 0; i < numTeapotVerts; ++i )
        {
            dataPtr[i].position = vertices[i].position;
            dataPtr[i].normal = vertices[i].normal;
        }
		teapotVertexBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kNPFormat, numTeapotVerts, dataPtr,
                                                                                             kImmutableUsage);

        unsigned int numTeapotIndices = indices.size();
		UInt32* indexPtr = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32)* numTeapotIndices);
        for ( unsigned int i = 0; i < numTeapotIndices; ++i )
        {
            indexPtr[i] = indices[i];
        }

		teapotIndexBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numTeapotIndices, indexPtr,
                                                                                           kImmutableUsage);

		if (!teapotVertexBuffer || !teapotIndexBuffer)
		{
			if (teapotVertexBuffer)
			{
				IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotVertexBuffer);
				teapotVertexBuffer = 0;
			}
			if (teapotIndexBuffer)
			{
				IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotIndexBuffer);
				teapotIndexBuffer = 0;
			}
		}

		IvStackAllocator::mScratchAllocator->Reset(currentOffset);
	}

    IvShaderProgram* oldShader = NULL;
    if (useDefaultShader)
    {
	    // clear to default shader
        oldShader = IvRenderer::mRenderer->GetShaderProgram();
	    IvRenderer::mRenderer->SetShaderProgram(0);

	    IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    }
    // draw it
    IvRenderer::mRenderer->Draw(kTriangleListPrim, teapotVertexBuffer, teapotIndexBuffer);

    if (useDefaultShader)
    {
	    // restore original state
	    IvRenderer::mRenderer->SetDefaultDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	    IvRenderer::mRenderer->SetShaderProgram( oldShader );
    }
}   // End of IvDrawTeapot()

/*
//-------------------------------------------------------------------------------
// @ IvDrawTexturedTeapot()
//-------------------------------------------------------------------------------
// Draw teapot, oriented with z pointing up
//-------------------------------------------------------------------------------
void 
IvDrawTexturedTeapot( bool useDefaultShader )
{
	// load data if not there
	if ( teapotVertexBuffer == 0 )
	{
        std::vector<IvTNPVertex> vertices;
        std::vector<unsigned int> indices;

        BuildTeapot( vertices, indices );

        unsigned int numTeapotVerts = vertices.size();
		teapotVertexBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kTNPFormat, numTeapotVerts);
		IvTNPVertex* dataPtr = (IvTNPVertex*) teapotVertexBuffer->BeginLoadData();
        
        for ( unsigned int i = 0; i < numTeapotVerts; ++i )
        {
            dataPtr[i].position = vertices[i].position;
            dataPtr[i].normal = vertices[i].normal;
            dataPtr[i].texturecoord = vertices[i].texturecoord;
        }
		if (!teapotVertexBuffer->EndLoadData())
		{
			IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotVertexBuffer);
			teapotVertexBuffer = 0;
		}

        unsigned int numTeapotIndices = indices.size();
		teapotIndexBuffer = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numTeapotIndices);
		unsigned int* indexPtr = (unsigned int*) teapotIndexBuffer->BeginLoadData();
        for ( unsigned int i = 0; i < numTeapotIndices; ++i )
        {
            indexPtr[i] = indices[i];
        }
		if (!teapotIndexBuffer->EndLoadData())
		{
			IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotVertexBuffer);
			teapotVertexBuffer = 0;
			IvRenderer::mRenderer->GetResourceManager()->Destroy(teapotIndexBuffer);
			teapotIndexBuffer = 0;
		}
	}

    IvShaderProgram* oldShader = NULL;
    if (useDefaultShader)
    {
	    // clear to default shader
        oldShader = IvRenderer::mRenderer->GetShaderProgram();
	    IvRenderer::mRenderer->SetShaderProgram(0);
    }
    IvRenderer::mRenderer->Draw(kTriangleListPrim, teapotVertexBuffer, teapotIndexBuffer);

    if (useDefaultShader)
    {
	    // restore original state
	    IvRenderer::mRenderer->SetShaderProgram( oldShader );
    }
}   // End of IvDrawTexturedTeapot()
*/
//-------------------------------------------------------------------------------
// @ IvSetDefaultLighting()
//-------------------------------------------------------------------------------
// Set up a default lighting scheme
//-------------------------------------------------------------------------------
void 
IvSetDefaultLighting()
{
    IvVector3 dir(-1.0f, 0.5f, 0.25f);
    dir.Normalize();
	IvRenderer::mRenderer->SetDefaultLightDirection(dir);
	IvRenderer::mRenderer->SetDefaultLightAmbient(0.4f, 0.4f, 0.4f, 1.0f);
	IvRenderer::mRenderer->SetDefaultLightDiffuse(0.25f, 0.25f, 0.25f, 1.0f);

}   // End of IvSetDefaultLighting()


//-------------------------------------------------------------------------------
// @ IvSetWorldMatrix()
//-------------------------------------------------------------------------------
// Set current world matrix
//-------------------------------------------------------------------------------
void 
IvSetWorldMatrix( const IvMatrix44& world )
{
    IvRenderer::mRenderer->SetWorldMatrix(world);

}   // End of IvSetWorldMatrix()


//-------------------------------------------------------------------------------
// @ IvSetWorldIdentity()
//-------------------------------------------------------------------------------
// Set current world matrix
//-------------------------------------------------------------------------------
void 
IvSetWorldIdentity()
{
    IvMatrix44 ident;

    IvRenderer::mRenderer->SetWorldMatrix(ident);

}   // End of IvSetWorldIdentity()


//-------------------------------------------------------------------------------
// @ IvSetViewMatrix()
//-------------------------------------------------------------------------------
// Set current view matrix
//-------------------------------------------------------------------------------
void 
IvSetViewMatrix( const IvMatrix44& view )
{
    IvRenderer::mRenderer->SetViewMatrix(view);

}   // End of IvSetViewMatrix()


//-------------------------------------------------------------------------------
// @ IvSetViewMatrix()
//-------------------------------------------------------------------------------
// Set current projection matrix
//-------------------------------------------------------------------------------
void 
IvSetProjectionMatrix( const IvMatrix44& projection )
{
    IvRenderer::mRenderer->SetProjectionMatrix(projection);

}   // End of IvSetProjectionMatrix()


//-------------------------------------------------------------------------------
// @ IvSetViewport()
//-------------------------------------------------------------------------------
// Set current viewport window
//-------------------------------------------------------------------------------
void 
IvSetViewport()
{

}   // End of IvSetViewport()



