//===============================================================================
// @ IvGeometry.h
// 
// Abstract scene graph object that supports rendering a geometry
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

#ifndef __IvGeometry__h__
#define __IvGeometry__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include <IvCapsule.h>
#include "IvSpatial.h"
#include <IvWriter.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvGeometry : public IvSpatial
{
public:
    // constructor/destructor
    IvGeometry();
    virtual ~IvGeometry();

    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvGeometry& source );

    virtual void UpdateWorldTransform();

    virtual void Render();

    const IvCapsule& GetWorldCapsule() const { return mWorldCapsule; }
  
    const IvCapsule& GetModelCapsule() const { return mModelCapsule; }
    void SetModelCapsule(const IvCapsule &capsule) { mModelCapsule = capsule; }
    
protected:
    IvCapsule mModelCapsule;
    IvCapsule mWorldCapsule;
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
