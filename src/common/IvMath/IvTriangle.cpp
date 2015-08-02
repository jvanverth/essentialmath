//----------------------------------------------------------------------------
// @ IvTriangle.cpp
// 
// Helpful routines for triangle data
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//----------------------------------------------------------------------------
//-- Includes ----------------------------------------------------------------
//----------------------------------------------------------------------------

#include "IvTriangle.h"
#include "IvVector3.h"
#include "IvVector2.h"
#include "IvRay3.h"
#include "IvMath.h"
#include "IvPlane.h"

//----------------------------------------------------------------------------
//-- Functions ---------------------------------------------------------------
//----------------------------------------------------------------------------

// helper functions
inline bool AdjustQ( const IvVector3& P0, const IvVector3& P1, 
                     const IvVector3& P2, const IvVector3& Q0, 
                     const IvVector3& Q1, const IvVector3& Q2,
                     float testQ0, float testQ1, float testQ2,
                     const IvVector3& normalP );
inline bool TestLineOverlap( const IvVector3& P0, const IvVector3& P1, 
                     const IvVector3& P2, const IvVector3& Q0, 
                     const IvVector3& Q1, const IvVector3& Q2 );
inline bool CoplanarTriangleIntersect( const IvVector3& P0, 
                     const IvVector3& P1, const IvVector3& P2, 
                     const IvVector3& Q0, const IvVector3& Q1, 
                     const IvVector3& Q2, const IvVector3& planeNormal );

inline bool EdgeTest( const IvVector2& edgePoint, const IvVector2& edgeVector, 
       float n, const IvVector2& P0, const IvVector2& P1, const IvVector2& P2 );

//-------------------------------------------------------------------------------
// @ ::IsPointInTriangle()
//-------------------------------------------------------------------------------
// Returns true if point on triangle plane lies inside triangle (3D version)
// Assumes triangle is not degenerate
//-------------------------------------------------------------------------------
bool IsPointInTriangle( const IvVector3& point, const IvVector3& P0,
                        const IvVector3& P1, const IvVector3& P2 )
{
    IvVector3 v0 = P1 - P0;
    IvVector3 v1 = P2 - P1;
    IvVector3 n = v0.Cross(v1);

    IvVector3 wTest = v0.Cross(point - P0);
    if ( wTest.Dot(n) < 0.0f )
    {
        return false;
    }

    wTest = v1.Cross(point - P1);
    if ( wTest.Dot(n) < 0.0f )
    {
        return false;
    }

    IvVector3 v2 = P0 - P2;
    wTest = v2.Cross(point - P2);
    if ( wTest.Dot(n) < 0.0f )
    {
        return false;
    }

    return true;
}


//-------------------------------------------------------------------------------
// @ ::BarycentricCoordinates()
//-------------------------------------------------------------------------------
// Returns barycentric coordinates for point inside triangle (3D version)
// Assumes triangle is not degenerate
//-------------------------------------------------------------------------------
void BarycentricCoordinates( float &r, float &s, float& t, 
                             const IvVector3& point, const IvVector3& P0,
                             const IvVector3& P1, const IvVector3& P2 )
{
    // get difference vectors
    IvVector3 u = P1 - P0;
    IvVector3 v = P2 - P0;
    IvVector3 w = point - P0;

    // compute cross product to get area of parallelograms
    IvVector3 a = u.Cross(w);
    IvVector3 b = v.Cross(w);
    IvVector3 c = u.Cross(v);
    
    // compute barycentric coordinates as ratios of areas
    float denom = 1.0f/c.Length();
    s = b.Length()*denom;
    t = a.Length()*denom;
    r = 1.0f - s - t;
}


//-------------------------------------------------------------------------------
// @ ::TriangleIntersect()
//-------------------------------------------------------------------------------
// Returns true if triangles P0P1P2 and Q0Q1Q2 intersect
// Assumes triangle is not degenerate
//
// This is not the algorithm presented in the text.  Instead, it is based on a 
// recent article by Guigue and Devillers in the July 2003 issue Journal of 
// Graphics Tools.  As it is faster than the ERIT algorithm, under ordinary 
// circumstances it would have been discussed in the text, but it arrived too late.  
//
// More information and the original source code can be found at
// http://www.acm.org/jgt/papers/GuigueDevillers03/
//
// A nearly identical algorithm was in the same issue of JGT, by Shen Heng and 
// Tang.  See http://www.acm.org/jgt/papers/ShenHengTang03/ for source code.
//
// Yes, this is complicated.  Did you think testing triangles would be easy?
//-------------------------------------------------------------------------------
bool TriangleIntersect( const IvVector3& P0, const IvVector3& P1, 
                        const IvVector3& P2, const IvVector3& Q0, 
                        const IvVector3& Q1, const IvVector3& Q2 )
{
    // test P against Q's plane
    IvVector3 normalQ = (Q1-Q0).Cross(Q2-Q0);

    float testP0 = normalQ.Dot( P0 - Q0 );
    float testP1 = normalQ.Dot( P1 - Q0 );
    float testP2 = normalQ.Dot( P2 - Q0 );
  
    // P doesn't intersect Q's plane
    if (testP0*testP1 > kEpsilon && testP0*testP2 > kEpsilon )
        return false;

    // test Q against P's plane
    IvVector3 normalP = (P1-P0).Cross(P2-P0);

    float testQ0 = normalP.Dot( Q0 - P0 );
    float testQ1 = normalP.Dot( Q1 - P0 );
    float testQ2 = normalP.Dot( Q2 - P0 );
  
    // Q doesn't intersect P's plane
    if (testQ0*testQ1 > kEpsilon && testQ0*testQ2 > kEpsilon )
        return false;
    
    // now we rearrange P's vertices such that the lone vertex (the one that lies
    // in its own half-space of Q) is first.  We also permute the other
    // triangle's vertices so that P0 will "see" them in counterclockwise order

    // Once reordered, we pass the vertices down to a helper function which will
    // reorder Q's vertices, and then test

    // P0 in Q's positive half-space
    if (testP0 > kEpsilon) 
    {
        // P1 in Q's positive half-space (so P2 is lone vertex)
        if (testP1 > kEpsilon) 
            return AdjustQ(P2, P0, P1, Q0, Q2, Q1, testQ0, testQ2, testQ1, normalP);
        // P2 in Q's positive half-space (so P1 is lone vertex)
        else if (testP2 > kEpsilon) 
            return AdjustQ(P1, P2, P0, Q0, Q2, Q1, testQ0, testQ2, testQ1, normalP);    
        // P0 is lone vertex
        else 
            return AdjustQ(P0, P1, P2, Q0, Q1, Q2, testQ0, testQ1, testQ2, normalP);
    } 
    // P0 in Q's negative half-space
    else if (testP0 < -kEpsilon) 
    {
        // P1 in Q's negative half-space (so P2 is lone vertex)
        if (testP1 < -kEpsilon) 
            return AdjustQ(P2, P0, P1, Q0, Q1, Q2, testQ0, testQ1, testQ2, normalP);
        // P2 in Q's negative half-space (so P1 is lone vertex)
        else if (testP2 < -kEpsilon) 
            return AdjustQ(P1, P2, P0, Q0, Q1, Q2, testQ0, testQ1, testQ2, normalP);
        // P0 is lone vertex
        else 
            return AdjustQ(P0, P1, P2, Q0, Q2, Q1, testQ0, testQ2, testQ1, normalP);
    } 
    // P0 on Q's plane
    else 
    {
        // P1 in Q's negative half-space 
        if (testP1 < -kEpsilon) 
        {
            // P2 in Q's negative half-space (P0 is lone vertex)
            if (testP2 < -kEpsilon) 
                return AdjustQ(P0, P1, P2, Q0, Q1, Q2, testQ0, testQ1, testQ2, normalP);
            // P2 in positive half-space or on plane (P1 is lone vertex)
            else 
                return AdjustQ(P1, P2, P0, Q0, Q2, Q1, testQ0, testQ2, testQ1, normalP);
        }
        // P1 in Q's positive half-space 
        else if (testP1 > kEpsilon) 
        {
            // P2 in Q's positive half-space (P0 is lone vertex)
            if (testP2 > kEpsilon) 
                return AdjustQ(P0, P1, P2, Q0, Q2, Q1, testQ0, testQ2, testQ1, normalP);
            // P2 in negative half-space or on plane (P1 is lone vertex)
            else 
                return AdjustQ(P1, P2, P0, Q0, Q1, Q2, testQ0, testQ1, testQ2, normalP);
        }
        // P1 lies on Q's plane too
        else  
        {
            // P2 in Q's positive half-space (P2 is lone vertex)
            if (testP2 > kEpsilon) 
                return AdjustQ(P2, P0, P1, Q0, Q1, Q2, testQ0, testQ1, testQ2, normalP);
            // P2 in Q's negative half-space (P2 is lone vertex)
            // note different ordering for Q vertices
            else if (testP2 < -kEpsilon) 
                return AdjustQ(P2, P0, P1, Q0, Q2, Q1, testQ0, testQ2, testQ1, normalP);
            // all three points lie on Q's plane, default to 2D test
            else 
                return CoplanarTriangleIntersect(P0, P1, P2, Q0, Q1, Q2, normalP);
        }
    }
}


//-------------------------------------------------------------------------------
// @ ::AdjustQ()
//-------------------------------------------------------------------------------
// Helper for TriangleIntersect()
//
// Now we rearrange Q's vertices such that the lone vertex (the one that lies
// in its own half-space of P) is first.  We also permute the other
// triangle's vertices so that Q0 will "see" them in counterclockwise order
//
// Once reordered, we pass the vertices down to a helper function which will
// actually test for intersection on the common line between the two planes
//-------------------------------------------------------------------------------
inline bool 
AdjustQ( const IvVector3& P0, const IvVector3& P1, 
                   const IvVector3& P2, const IvVector3& Q0, 
                   const IvVector3& Q1, const IvVector3& Q2,
                   float testQ0, float testQ1, float testQ2,
                   const IvVector3& normalP )
{

    // Q0 in P's positive half-space
    if (testQ0 > kEpsilon) 
    { 
        // Q1 in P's positive half-space (so Q2 is lone vertex)
        if (testQ1 > kEpsilon) 
            return TestLineOverlap(P0, P2, P1, Q2, Q0, Q1);
        // Q2 in P's positive half-space (so Q1 is lone vertex)
        else if (testQ2 > kEpsilon) 
            return TestLineOverlap(P0, P2, P1, Q1, Q2, Q0);
        // Q0 is lone vertex
        else 
            return TestLineOverlap(P0, P1, P2, Q0, Q1, Q2);
    }
    // Q0 in P's negative half-space
    else if (testQ0 < -kEpsilon) 
    { 
        // Q1 in P's negative half-space (so Q2 is lone vertex)
        if (testQ1 < -kEpsilon) 
            return TestLineOverlap(P0, P1, P2, Q2, Q0, Q1);
        // Q2 in P's negative half-space (so Q1 is lone vertex)
        else if (testQ2 < -kEpsilon) 
            return TestLineOverlap(P0, P1, P2, Q1, Q2, Q0);
        // Q0 is lone vertex
        else 
            return TestLineOverlap(P0, P2, P1, Q0, Q1, Q2);
    }
    // Q0 on P's plane
    else 
    { 
        // Q1 in P's negative half-space 
        if (testQ1 < -kEpsilon) 
        { 
            // Q2 in P's negative half-space (Q0 is lone vertex)
            if (testQ2 < -kEpsilon)  
                return TestLineOverlap(P0, P1, P2, Q0, Q1, Q2);
            // Q2 in positive half-space or on plane (P1 is lone vertex)
            else 
                return TestLineOverlap(P0, P2, P1, Q1, Q2, Q0);
        }
        // Q1 in P's positive half-space 
        else if (testQ1 > kEpsilon) 
        { 
            // Q2 in P's positive half-space (Q0 is lone vertex)
            if (testQ2 > kEpsilon) 
                return TestLineOverlap(P0, P2, P1, Q0, Q1, Q2);
            // Q2 in negative half-space or on plane (P1 is lone vertex)
            else  
                return TestLineOverlap(P0, P1, P2, Q1, Q2, Q0);
        }
        // Q1 lies on P's plane too
        else 
        {
            // Q2 in P's positive half-space (Q2 is lone vertex)
            if (testQ2 > kEpsilon) 
                return TestLineOverlap(P0, P1, P2, Q2, Q0, Q1);
            // Q2 in P's negative half-space (Q2 is lone vertex)
            // note different ordering for Q vertices
            else if (testQ2 < -kEpsilon) 
                return TestLineOverlap(P0, P2, P1, Q2, Q0, Q1);
            // all three points lie on P's plane, default to 2D test
            else 
                return CoplanarTriangleIntersect(P0, P1, P2, Q0, Q1, Q2, normalP);
        }
    }
}


//-------------------------------------------------------------------------------
// @ ::TestLineOverlap()
//-------------------------------------------------------------------------------
// Helper for TriangleIntersect()
//
// This tests whether the rearranged triangles overlap, by checking the intervals
// where their edges cross the common line between the two planes.  If the 
// interval for P is [i,j] and Q is [k,l], then there is intersection if the
// intervals overlap.  Previous algorithms computed these intervals directly, 
// this tests implictly by using two "plane tests."
//-------------------------------------------------------------------------------
inline bool 
TestLineOverlap( const IvVector3& P0, const IvVector3& P1, 
                   const IvVector3& P2, const IvVector3& Q0, 
                   const IvVector3& Q1, const IvVector3& Q2 )
{
    // get "plane normal"
    IvVector3 normal = (P1-P0).Cross(Q0-P0);

    // fails test, no intersection
    if ( normal.Dot( Q1 - P0 ) > kEpsilon )
        return false;
  
    // get "plane normal"
    normal = (P2-P0).Cross(Q2-P0);

    // fails test, no intersection
    if ( normal.Dot( Q0 - P0 ) > kEpsilon )
        return false;

    // intersection!
    return true;
}


//-------------------------------------------------------------------------------
// @ ::CoplanarTriangleIntersect()
//-------------------------------------------------------------------------------
// Helper for TriangleIntersect()
//
// This projects the two triangles down to 2D, maintaining the largest area by
// dropping the dimension where the normal points the farthest.
//-------------------------------------------------------------------------------
inline bool CoplanarTriangleIntersect( const IvVector3& P0, 
                     const IvVector3& P1, const IvVector3& P2, 
                     const IvVector3& Q0, const IvVector3& Q1, 
                     const IvVector3& Q2, const IvVector3& planeNormal )
{
    IvVector3 absNormal( ::IvAbs(planeNormal.x), ::IvAbs(planeNormal.y), 
                        ::IvAbs(planeNormal.z) );

    IvVector2 projP0, projP1, projP2;
    IvVector2 projQ0, projQ1, projQ2;

    // if x is direction of largest magnitude
    if ( absNormal.x > absNormal.y && absNormal.x >= absNormal.z )
    {
        projP0.Set( P0.y, P0.z );
        projP1.Set( P1.y, P1.z );
        projP2.Set( P2.y, P2.z );
        projQ0.Set( Q0.y, Q0.z );
        projQ1.Set( Q1.y, Q1.z );
        projQ2.Set( Q2.y, Q2.z );
    }
    // if y is direction of largest magnitude
    else if ( absNormal.y > absNormal.x && absNormal.y >= absNormal.z )
    {
        projP0.Set( P0.x, P0.z );
        projP1.Set( P1.x, P1.z );
        projP2.Set( P2.x, P2.z );
        projQ0.Set( Q0.x, Q0.z );
        projQ1.Set( Q1.x, Q1.z );
        projQ2.Set( Q2.x, Q2.z );
    }
    // z is the direction of largest magnitude
    else
    {
        projP0.Set( P0.x, P0.y );
        projP1.Set( P1.x, P1.y );
        projP2.Set( P2.x, P2.y );
        projQ0.Set( Q0.x, Q0.y );
        projQ1.Set( Q1.x, Q1.y );
        projQ2.Set( Q2.x, Q2.y );
    }

    return TriangleIntersect( projP0, projP1, projP2, projQ0, projQ1, projQ2 );
}



//-------------------------------------------------------------------------------
// @ ::TriangleIntersect()
//-------------------------------------------------------------------------------
// Returns true if ray intersects triangle
//-------------------------------------------------------------------------------
bool
TriangleIntersect( float& t, const IvVector3& P0, const IvVector3& P1, 
                   const IvVector3& P2, const IvRay3& ray )
{
    // test ray direction against triangle
    IvVector3 e1 = P1 - P0;
    IvVector3 e2 = P2 - P0;
    IvVector3 p = ray.GetDirection().Cross(e2);
    float a = e1.Dot(p);

    // if result zero, no intersection or infinite intersections
    // (ray parallel to triangle plane)
    if ( IvIsZero(a) )
        return false;

    // compute denominator
    float f = 1.0f/a;

    // compute barycentric coordinates
    IvVector3 s = ray.GetOrigin() - P0;
    float u = f*s.Dot(p);

    // ray falls outside triangle
    if (u < 0.0f || u > 1.0f) 
        return false;

    IvVector3 q = s.Cross(e1);
    float v = f*ray.GetDirection().Dot(q);

    // ray falls outside triangle
    if (v < 0.0f || u+v > 1.0f) 
        return false;

    // compute line parameter
    t = f*e2.Dot(q);

    return (t >= 0.0f);
}


//-------------------------------------------------------------------------------
// @ ::TriangleClassify()
//-------------------------------------------------------------------------------
// Returns signed distance between plane and triangle
//-------------------------------------------------------------------------------
float
TriangleClassify( const IvVector3& P0, const IvVector3& P1, 
                  const IvVector3& P2, const IvPlane& plane )
{
    float test0 = plane.Test( P0 );
    float test1 = plane.Test( P1 );

    // if two points lie on opposite sides of plane, intersect
    if (test0*test1 < 0.0f)
        return 0.0f;

    float test2 = plane.Test( P2 );

    // if two points lie on opposite sides of plane, intersect
    if (test0*test2 < 0.0f)
        return 0.0f;
    if (test1*test2 < 0.0f)
        return 0.0f;

    // no intersection, return signed distance
    if ( test0 < 0.0f )
    {
        if ( test0 < test1 )
        {
            if ( test1 < test2 )
                return test2;
            else
                return test1;
        }
        else if (test0 < test2)
        {
            return test2;
        }
        else
        {   
            return test0;
        }
    }
    else
    {
        if ( test0 > test1 )
        {
            if ( test1 > test2 )
                return test2;
            else
                return test1;
        }
        else if (test0 > test2)
        {
            return test2;
        }
        else
        {   
            return test0;
        }
    }
    
}


//-------------------------------------------------------------------------------
// @ ::IsPointInTriangle()
//-------------------------------------------------------------------------------
// Returns true if point lies inside triangle (2D version)
// Assumes triangle is not degenerate
//-------------------------------------------------------------------------------
bool IsPointInTriangle( const IvVector2& point, const IvVector2& P0,
                        const IvVector2& P1, const IvVector2& P2 )
{
    IvVector2 v0 = P1 - P0;
    IvVector2 v1 = P2 - P1;
    float n = v0.PerpDot(v1);

    float wTest = v0.PerpDot(point - P0);
    if ( wTest*n < 0.0f )
    {
        return false;
    }

    wTest = v1.PerpDot(point - P1);
    if ( wTest*n < 0.0f )
    {
        return false;
    }

    IvVector2 v2 = P0 - P2;
    wTest = v2.PerpDot(point - P2);
    if ( wTest*n < 0.0f )
    {
        return false;
    }

    return true;

}   // End of IsPointInTriangle()


//-------------------------------------------------------------------------------
// @ ::BarycentricCoordinates()
//-------------------------------------------------------------------------------
// Returns barycentric coordinates for point inside triangle (2D version)
// Assumes triangle is not degenerate
//-------------------------------------------------------------------------------
void BarycentricCoordinates( float &r, float &s, float& t, 
                             const IvVector2& point, const IvVector2& P0,
                             const IvVector2& P1, const IvVector2& P2 )
{
    // get difference vectors
    IvVector2 u = P1 - P0;
    IvVector2 v = P2 - P0;
    IvVector2 w = point - P0;

    // compute perpendicular dot product to get area of parallelograms
    float a = u.PerpDot(w);
    float b = v.PerpDot(w);
    float c = u.PerpDot(v);
    
    // compute barycentric coordinates as ratios of areas
    float denom = 1.0f/c;
    s = b*denom;
    t = a*denom;
    r = 1.0f - s - t;

}   // End of BarycentricCoordinates()


//-------------------------------------------------------------------------------
// @ ::TriangleIntersect()
//-------------------------------------------------------------------------------
// Returns true if triangles P0P1P2 and Q0Q1Q2 intersect (2D version)
// Assumes triangle is not degenerate
//
// Uses principle of separating planes again: if all three points of one triangle
// lie outside one edge of the other, then they are disjoint
//
// This could probably be made faster.  
// See // http://www.acm.org/jgt/papers/GuigueDevillers03/ for another possible
// implementation.
//-------------------------------------------------------------------------------
bool TriangleIntersect( const IvVector2& P0, const IvVector2& P1, 
                        const IvVector2& P2, const IvVector2& Q0, 
                        const IvVector2& Q1, const IvVector2& Q2 )
{
    // test Q against P
    IvVector2 v0 = P1 - P0;
    IvVector2 v1 = P2 - P1;
    float n = v0.PerpDot(v1);

    // test against edge 0
    if (EdgeTest( P0, v0, n, Q0, Q1, Q2 ))
        return false;

    // test against edge 1
    if (EdgeTest( P1, v1, n, Q0, Q1, Q2 ))
        return false;

    // test against edge 2
    IvVector2 v2 = P0 - P2;
    if (EdgeTest( P2, v2, n, Q0, Q1, Q2 ))
        return false;

    // test P against Q
    v0 = Q1 - Q0;
    v1 = Q2 - Q1;
    n = v0.PerpDot(v1);

    // test against edge 0
    if (EdgeTest( Q0, v0, n, P0, P1, P2 ))
        return false;

    // test against edge 1
    if (EdgeTest( Q1, v1, n, P0, P1, P2 ))
        return false;

    // test against edge 2
    v2 = Q0 - Q2;
    if (EdgeTest( Q2, v2, n, P0, P1, P2 ))
        return false;

    return true;
}


//-------------------------------------------------------------------------------
// @ ::EdgeTest()
//-------------------------------------------------------------------------------
// Helper function for 2D TriangleIntersect()
//
// Given an edge vector and origin, triangle "normal" value, and three points,
// determine if all three points lie outside the edge
//-------------------------------------------------------------------------------
inline bool 
EdgeTest( const IvVector2& edgePoint, const IvVector2& edgeVector, float n, 
          const IvVector2& P0, const IvVector2& P1, const IvVector2& P2 )
{
    // test each point against the edge in turn
    float wTest = edgeVector.PerpDot(P0 - edgePoint);
    if ( wTest*n > 0.0f )
    {
        return false;
    }
    wTest = edgeVector.PerpDot(P1 - edgePoint);
    if ( wTest*n > 0.0f )
    {
        return false;
    }
    wTest = edgeVector.PerpDot(P2 - edgePoint);
    if ( wTest*n > 0.0f )
    {
        return false;
    }

    return true;
}
