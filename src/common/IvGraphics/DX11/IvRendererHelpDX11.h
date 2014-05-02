//===============================================================================
// @ IvGLHelp.h
// 
// Helper routines for setting up basic GL functionality
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvGLHelp__h__
#define __IvGLHelp__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include <d3d9.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class IvMatrix44;
class IvMatrix33;
class IvLineSegment3;

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

// draw standard shapes
void IvDrawCube();
void IvDrawBox( const IvVector3& minima, const IvVector3& maxima );
void IvDrawOBB( const IvVector3& extents, const IvVector3& center, const IvMatrix33& axes );
void IvDrawTeapot();
void IvDrawSphere( float radius );
void IvDrawCapsule( const IvLineSegment3& segment, float radius );
void IvDrawLine( const IvVector3& from, const IvVector3& to );

// draw standard environment
void IvDrawAxes();
void IvDrawFloor();

// draw indexed data
void IvDrawArrays( const IvVector3* verts, const IvVector3* normals,
                   const IvVector3* colors, unsigned int* indices,
                   unsigned int numIndices );

// set up some defaults
void IvSetDefaultViewer( float xPos, float yPos, float zPos );
void IvSetDefaultLighting();

// set up graphics pipeline
void IvSetWorldMatrix( const IvMatrix44& world );
void IvSetWorldIdentity();
void IvSetViewMatrix( const IvMatrix44& view );
void IvSetProjectionMatrix( const IvMatrix44& projection );
void IvSetViewport();

#endif
