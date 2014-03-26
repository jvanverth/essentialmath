//===============================================================================
// @ IvSpatial.cpp
// ------------------------------------------------------------------------------
// Abstract scene graph object that supports hierarchical transforms
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvSpatial.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

bool IvSpatial::gDisplayHierarchyBounds = false;
bool IvSpatial::gDisplayLeafBounds = false;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvSpatial::IvSpatial()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvSpatial::IvSpatial()
{
    mParent = 0;

    mLocalScale = 1.0f;
    mLocalRotate.Identity();
    mLocalTranslate.Zero();

    mWorldScale = 1.0f;
    mWorldRotate.Identity();
    mWorldTranslate.Zero();

}  // End of IvSpatial::IvSpatial


//-------------------------------------------------------------------------------
// @ IvSpatial::~IvSpatial()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvSpatial::~IvSpatial()
{
    /* */
}  // End of IvSpatial::~IvSpatial


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvSpatial& source)
{
    out << ((source.mParent != 0) ? "Has Parent" : "No Parent") << eol;

    out << source.mLocalScale << eol;
    out << source.mLocalRotate << source.mLocalTranslate << eol;
    
    out << source.mWorldScale << eol;
    out << source.mWorldRotate << source.mWorldTranslate << eol;
    out << source.mWorldSphere << eol;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvSpatial::UpdateWorldTransform()
//-------------------------------------------------------------------------------
// Updates the world transforms of the object, based on the local and parent
// transforms (if any)
//-------------------------------------------------------------------------------
void IvSpatial::UpdateWorldTransform()
{
    if (mParent)
    {
        mWorldScale = mParent->mWorldScale * mLocalScale;
        mWorldRotate = mParent->mWorldRotate * mLocalRotate;
        mWorldTranslate = mParent->mWorldTranslate 
            + mParent->mWorldScale * mParent->mWorldRotate * mLocalTranslate;
    }
    else
    {
        mWorldScale = mLocalScale;
        mWorldRotate = mLocalRotate;
        mWorldTranslate = mLocalTranslate;
    }
}  // End of IvSpatial::UpdateWorldTransform


