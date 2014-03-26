//===============================================================================
// @ IvNode.cpp
// ------------------------------------------------------------------------------
// Scene graph object that supports multiple child objects
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvNode.h"
#include <IvMatrix44.h>
#include <IvRendererHelp.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvNode::IvNode()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvNode::IvNode(unsigned int numChildren) : IvSpatial()
{
    mNumChildren = numChildren;

    mChildren = new IvSpatial*[mNumChildren];

    unsigned int i;
    for (i = 0; i < mNumChildren; i++)
        mChildren[i] = 0;

}  // End of IvNode::IvNode


//-------------------------------------------------------------------------------
// @ IvNode::~IvNode()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvNode::~IvNode()
{
    unsigned int i;
    for ( i = 0; i < mNumChildren; i++)
    {
        if (mChildren[i])
        {
            mChildren[i]->SetParent(0);
            delete mChildren[i];
        }
    }

    delete[] mChildren;

}  // End of IvNode::~IvNode


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvNode& source)
{
    // call base class
    out << ((IvSpatial&)source);

    out << source.mNumChildren << eol;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ IvNode::UpdateWorldTransform()
//-------------------------------------------------------------------------------
// Updates the world transform of the object and its children
//-------------------------------------------------------------------------------
void IvNode::UpdateWorldTransform()
{
    IvSpatial::UpdateWorldTransform();
    
    unsigned int i;
    for ( i = 0; i < mNumChildren; i++ )
    {
        if (mChildren[i])
            mChildren[i]->UpdateWorldTransform();
    }

    // update the world bound by merging the bounds of the children.  First,
    // we find the first non-zero child and set the world bound to that
    for ( i = 0; i < mNumChildren; i++ )
    {
        if (mChildren[i])
        {
            // set the bound to the first child and then break
            mWorldSphere = mChildren[i]->GetWorldBoundingSphere();
            break;
        }
    }

    // do not iterate from 0 - iterate from the next child onward, merging the
    // existing bound with the next child
    for ( ; i < mNumChildren; i++ )
    {
        if (mChildren[i])
        {
            Merge( mWorldSphere, mWorldSphere, 
                mChildren[i]->GetWorldBoundingSphere() );
        }
    }

}  // End of IvNode::UpdateWorldTransform


//-------------------------------------------------------------------------------
// @ IvNode::Render()
//-------------------------------------------------------------------------------
// Draws all of the children of the current object, in order
//-------------------------------------------------------------------------------
void IvNode::Render()
{
    unsigned int i;
    for ( i = 0; i < mNumChildren; ++i )
    {
        if (mChildren[i])
            mChildren[i]->Render();
    }

/*    if (gDisplayHierarchyBounds)
    {
        glPolygonMode(GL_FRONT, GL_LINE);
    
        IvMatrix44 ident;
        ident(0,3) = mWorldSphere.GetCenter().x;
        ident(1,3) = mWorldSphere.GetCenter().y;
        ident(2,3) = mWorldSphere.GetCenter().z;

        IvSetWorldMatrix(ident);
        IvSetColor( 1.0f, 1.0f, 0.0f );
        IvDrawSphere(mWorldSphere.GetRadius());

        glPopMatrix();

        glPolygonMode(GL_FRONT, GL_FILL);
    }
*/
}  // End of IvNode::Render


//-------------------------------------------------------------------------------
// @ IvNode::SetChild()
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void IvNode::SetChild(unsigned int index, IvSpatial* child)
{
    // must detach this as the parent of the current child at the given index
    if (mChildren[index])
        mChildren[index]->SetParent(0);
    
    // set this as the parent of the new child at the given index
    if (child)
        child->SetParent(this);
    
    // set the new child
    mChildren[index] = child;
}  // End of IvNode::SetChild
