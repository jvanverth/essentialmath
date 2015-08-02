//===============================================================================
// @ IvRendererHelp.h
// 
// Helper routines for setting up basic Renderer functionality
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//===============================================================================

#pragma once
#ifndef __IvRendererHelp__h__
#define __IvRendererHelp__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvColor.h"

class IvVector3;
class IvMatrix44;
class IvMatrix33;
class IvLineSegment3;

extern IvColor kRed;
extern IvColor kGreen;
extern IvColor kBlue;
extern IvColor kMagenta;
extern IvColor kYellow;
extern IvColor kCyan;
extern IvColor kOrange;
extern IvColor kWhite;
extern IvColor kBlack;
extern IvColor kGrey;

// build/destroy the vertex buffers
void IvCleanUpRendererHelpData();

// draw standard shapes
void IvDrawCube( IvColor color = kYellow );
void IvDrawBox( const IvVector3& minima, const IvVector3& maxima, IvColor color = kWhite );
void IvDrawBoxNoShader( const IvVector3& minima, const IvVector3& maxima, IvColor color = kWhite );
void IvDrawOBB( const IvVector3& extents, const IvVector3& center, const IvMatrix33& axes, IvColor color = kWhite );
void IvDrawUnitSphere();
void IvDrawSphere( float radius, IvColor color = kCyan );
void IvDrawCapsule( const IvLineSegment3& segment, float radius, IvColor color = kYellow );
void IvDrawLine( const IvVector3& from, const IvVector3& to, IvColor color = kWhite );
void IvDrawTeapot( IvColor color = kMagenta, bool useDefaultShader = true );
void IvDrawTexturedTeapot( bool useDefaultShader = true );

// draw standard environment
void IvDrawAxes();
void IvDrawFloor();

// set up some defaults
void IvSetDefaultViewer( float xPos, float yPos, float zPos );
void IvSetDefaultLighting();

// matrices
void IvSetWorldMatrix( const IvMatrix44& world );
void IvSetWorldIdentity();
void IvSetViewMatrix( const IvMatrix44& view );
void IvSetProjectionMatrix( const IvMatrix44& projection );
void IvSetViewport();

#endif
