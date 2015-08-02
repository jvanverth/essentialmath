//===============================================================================
// @ IvGLHelp.h
// 
// Helper routines for setting up basic GL functionality
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

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

inline void IvSetColor( float r, float g, float b )
{
    glColor3f( r, g, b );
}

// set up graphics pipeline
void IvSetWorldIdentity();

#endif
