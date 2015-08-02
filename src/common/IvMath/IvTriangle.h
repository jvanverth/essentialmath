//===============================================================================
// @ IvTriangle.h
// 
// Useful triangle-related routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

#ifndef __IvTriangle__h__
#define __IvTriangle__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector2;
class IvVector3;
class IvRay3;
class IvPlane;

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

// 3D versions
bool IsPointInTriangle( const IvVector3& point, const IvVector3& P0,
                        const IvVector3& P1, const IvVector3& P2 );
void BarycentricCoordinates( float &r, float &s, float& t, 
                             const IvVector3& point, const IvVector3& P0,
                             const IvVector3& P1, const IvVector3& P2 );

// intersection
bool TriangleIntersect( const IvVector3& P0, const IvVector3& P1, 
                        const IvVector3& P2, const IvVector3& Q0, 
                        const IvVector3& Q1, const IvVector3& Q2 );
bool TriangleIntersect( float& t, const IvVector3& P0, const IvVector3& P1, 
                        const IvVector3& P2, const IvRay3& ray );

// plane classification
float TriangleClassify( const IvVector3& P0, const IvVector3& P1, 
                        const IvVector3& P2, const IvPlane& plane );

// 2D versions
bool IsPointInTriangle( const IvVector2& point, const IvVector2& P0,
                        const IvVector2& P1, const IvVector2& P2 );
void BarycentricCoordinates( float &r, float &s, float& t, 
                             const IvVector2& point, const IvVector2& P0,
                             const IvVector2& P1, const IvVector2& P2 );
bool TriangleIntersect( const IvVector2& P0, const IvVector2& P1, 
                        const IvVector2& P2, const IvVector2& Q0, 
                        const IvVector2& Q1, const IvVector2& Q2 );

#endif
