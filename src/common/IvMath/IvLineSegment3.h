//===============================================================================
// @ IvLineSegment3.h
// 
// Description
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

#ifndef __IvLineSegment3__h__
#define __IvLineSegment3__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvWriter.h"
#include "IvVector3.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;
class IvQuat;
class IvLine3;
class IvRay3;
class IvMatrix33;
class IvLineSegment3;

float DistanceSquared( const IvLineSegment3& seg0, const IvLineSegment3& seg1,
                      float& s_c, float& t_c );
float DistanceSquared( const IvLineSegment3& segment, const IvRay3& ray,
                       float& s_c, float& t_c );
float DistanceSquared( const IvLineSegment3& segment, const IvLine3& line, 
                       float& s_c, float& t_c );

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvLineSegment3
{
public:
    // constructor/destructor
    IvLineSegment3();
    IvLineSegment3( const IvVector3& endpoint0, const IvVector3& endpoint1 );
    inline ~IvLineSegment3() {}
    
    // copy operations
    IvLineSegment3(const IvLineSegment3& other);
    IvLineSegment3& operator=(const IvLineSegment3& other);

    // text output (for debugging)
    friend IvWriter& operator<<(IvWriter& out, const IvLineSegment3& source);

    // accessors
    float& operator()(unsigned int i, unsigned int j);
    float  operator()(unsigned int i, unsigned int j) const;

    inline IvVector3 GetOrigin() const { return mOrigin; }
    inline IvVector3 GetDirection() const { return mDirection; }
    inline IvVector3 GetEndpoint0() const { return mOrigin; }
    inline IvVector3 GetEndpoint1() const { return mOrigin + mDirection; }
    inline IvVector3 GetCenter() const { return mOrigin + 0.5f*mDirection; }
    void Get( IvVector3& end0, IvVector3& end1 ) const;

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const IvLineSegment3& segment ) const;
    bool operator!=( const IvLineSegment3& segment ) const;

    // manipulators
    void Set( const IvVector3& end0, const IvVector3& end1 );
    inline void Clean() { mOrigin.Clean(); mDirection.Clean(); }

    // transform!
    IvLineSegment3 Transform( float scale, const IvQuat& rotation, 
                              const IvVector3& translation ) const;
    IvLineSegment3 Transform( float scale, const IvMatrix33& rotation, 
                              const IvVector3& translation ) const;

    // distance
    friend float DistanceSquared( const IvLineSegment3& segment0, 
                                  const IvLineSegment3& segment1, 
                                  float& s_c, float& t_c );
    friend float DistanceSquared( const IvLineSegment3& segment, 
                                  const IvRay3& ray, 
                                  float& s_c, float& t_c );
    inline friend float DistanceSquared( const IvRay3& ray, 
                                  const IvLineSegment3& segment, 
                                  float& s_c, float& t_c )
    {
        return DistanceSquared( segment, ray, t_c, s_c );
    }
    friend float DistanceSquared( const IvLineSegment3& segment, 
                                  const IvLine3& line, 
                                  float& s_c, float& t_c );
    inline friend float DistanceSquared( const IvLine3& line, 
                                  const IvLineSegment3& segment, 
                                  float& s_c, float& t_c )
    {
        return DistanceSquared( segment, line, t_c, s_c );
    }
    friend float DistanceSquared( const IvLineSegment3& segment, 
                                  const IvVector3& point, 
                                  float& t_c );

    // closest points
    friend void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                               const IvLineSegment3& segment0, 
                               const IvLineSegment3& segment1 );
    friend void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                               const IvLineSegment3& segment, 
                               const IvRay3& ray );
    friend void ClosestPoints( IvVector3& point0, IvVector3& point1, 
                               const IvLineSegment3& segment, 
                               const IvLine3& line );
    IvVector3 ClosestPoint( const IvVector3& point ) const;
        
protected:
    IvVector3 mOrigin;
    IvVector3 mDirection;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
