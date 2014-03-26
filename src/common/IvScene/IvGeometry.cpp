//===============================================================================
// @ IvGeometry.cpp
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
#include "IvGeometry.h"
#include <IvRendererHelp.h>
#include <IvMatrix44.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvGeometry::IvGeometry()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
IvGeometry::IvGeometry() : IvSpatial()
{
    /* */
}  // End of IvGeometry::IvGeometry


//-------------------------------------------------------------------------------
// @ IvGeometry::~IvGeometry()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
IvGeometry::~IvGeometry()
{
    /* */
}  // End of IvGeometry::~IvGeometry


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvGeometry& source)
{
    // call base class
    out << ((IvSpatial&)source);

    out << source.mModelCapsule << eol;
    out << source.mWorldCapsule << eol;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvGeometry::UpdateWorldTransform()
//-------------------------------------------------------------------------------
// Updates the world transforms and bounds
//-------------------------------------------------------------------------------
void IvGeometry::UpdateWorldTransform()
{
    IvSpatial::UpdateWorldTransform();

    // compute the world capsule from the model capsule
    mWorldCapsule = mModelCapsule.Transform( mWorldScale, mWorldRotate,
        mWorldTranslate );

    // Bound the capsule with a sphere, centered at the midpoint, with radius
    // equal to the capsule radius plus half the capsule segment length
    mWorldSphere.SetCenter(mWorldCapsule.GetSegment().GetCenter());
    mWorldSphere.SetRadius(mWorldCapsule.GetSegment().Length() * 0.5f 
        + mWorldCapsule.GetRadius());

}  // End of IvGeometry::UpdateWorldTransform


//-------------------------------------------------------------------------------
// @ IvGeometry::Render()
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void IvGeometry::Render()
{
    // build 4x4 matrix
    IvMatrix44 transform( mWorldRotate );
    transform(0,0) *= mWorldScale;
    transform(1,0) *= mWorldScale;
    transform(2,0) *= mWorldScale;
    transform(0,1) *= mWorldScale;
    transform(1,1) *= mWorldScale;
    transform(2,1) *= mWorldScale;
    transform(0,2) *= mWorldScale;
    transform(1,2) *= mWorldScale;
    transform(2,2) *= mWorldScale;
    transform(0,3) = mWorldTranslate.GetX();
    transform(1,3) = mWorldTranslate.GetY();
    transform(2,3) = mWorldTranslate.GetZ();
    
    // set transform
    IvSetWorldMatrix( transform );


}  // End of IvGeometry::Render

