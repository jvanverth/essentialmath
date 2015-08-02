//===============================================================================
// @ ObjectDB.cpp
// ------------------------------------------------------------------------------
// Object database
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// This is the main class for this demo.  It manages a set of objects, and 
// detects collisions between them by using a sweep-and-prune method, which is
// handled in Update().  A list is created, storing the min and max x extents for
// each object, with a pointer to each object.  The list is sorted by the x 
// values.  Then we go through the extent list.  If it's a min extent, we check 
// for collisions in the active list, and then add the object to the active list.
// Otherwise we remove the object from the active list.
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRendererHelp.h>

#include "ObjectDB.h"
#include "CollisionObject.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ ObjectDB::ObjectDB()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
ObjectDB::ObjectDB() : 
    mNumObjects( 0 ),
    mMaxObjects( 0 ), 
    mObjects( 0 )
{
}   // End of ObjectDB::ObjectDB()


//-------------------------------------------------------------------------------
// @ ObjectDB::~ObjectDB()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
ObjectDB::~ObjectDB()
{
    Clean();

}   // End of ObjectDB::~ObjectDB()


//-------------------------------------------------------------------------------
// @ ObjectDB::Initialize()
//-------------------------------------------------------------------------------
// Initialize object database array
//-------------------------------------------------------------------------------
bool
ObjectDB::Initialize( unsigned int maxObjects )
{
    // something already in here
    if (mNumObjects > 0)
        return false;

    // initialize members for this maximum size
    mMaxObjects = maxObjects;
    mObjects = new CollisionObject*[maxObjects];
    mSweepData = new SweepPoint[2*maxObjects];
    mActiveObjects = new CollisionObject*[maxObjects];
    mNumObjects = 0;

    return true;
}


//-------------------------------------------------------------------------------
// @ ObjectDB::AddObject()
//-------------------------------------------------------------------------------
// Add object to database
//-------------------------------------------------------------------------------
bool
ObjectDB::AddObject( CollisionObject* object )
{
    // uninitialized or already full
    if ( mMaxObjects == 0 || mNumObjects == mMaxObjects )
        return false;

    // add sweep data objects
    mSweepData[ 2*mNumObjects ].mObject = object;
    mSweepData[ 2*mNumObjects ].mXValue = object->GetMinXExtent();
    mSweepData[ 2*mNumObjects ].mFinish = false;
    mSweepData[ 2*mNumObjects+1 ].mObject = object;
    mSweepData[ 2*mNumObjects+1 ].mXValue = object->GetMaxXExtent();
    mSweepData[ 2*mNumObjects+1 ].mFinish = true;

    // add new object to main list
    mObjects[ mNumObjects++ ] = object;

    return true;
}


//-------------------------------------------------------------------------------
// @ ObjectDB::Clean()
//-------------------------------------------------------------------------------
// Clean out database
//-------------------------------------------------------------------------------
void
ObjectDB::Clean()
{
    for ( unsigned int i = 0; i < mNumObjects; ++i )
    {
        delete mObjects[i];
    }

    delete [] mObjects;
    delete [] mSweepData;
    delete [] mActiveObjects;
    mObjects = 0;
    mMaxObjects = 0;
    mNumObjects = 0;
}


//-------------------------------------------------------------------------------
// @ ObjectDB::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
ObjectDB::Update( float dt )
{
    // update each object first, moving to new position
    unsigned int i;
    for ( i = 0; i < mNumObjects; ++i )
    {
        mObjects[i]->Update( dt );
    }

    // update sweep data
    for ( i = 0; i < 2*mNumObjects; ++i )
    {
        if ( !mSweepData[i].mFinish )
            mSweepData[i].mXValue = 
                mSweepData[i].mObject->GetMinXExtent();
        else
            mSweepData[i].mXValue = 
                mSweepData[i].mObject->GetMaxXExtent();
    }
    
    // bubble sort by x value
    for ( i = 0; i < 2*mNumObjects-1; ++i )
    {
        for ( unsigned int j = 0; j < 2*mNumObjects-1-i; ++j )
        {
            if ( mSweepData[j+1].mXValue < mSweepData[j].mXValue )
            {
                SweepPoint temp = mSweepData[j];
                mSweepData[j] = mSweepData[j+1];
                mSweepData[j+1] = temp;
            }
        }
    }

    // now check for collisions
    mNumActiveObjects = 0;
    for ( i = 0; i < 2*mNumObjects; ++i )
    {       
        // we're done with this one
        if ( mSweepData[i].mFinish )
        {
            // find this object in the list and remove it
            for ( unsigned int j = 0; j < mNumActiveObjects; ++j )
            {
                // if found, move last object to its spot
                if ( mActiveObjects[j] == mSweepData[i].mObject )
                {
                    mActiveObjects[j] = mActiveObjects[mNumActiveObjects-1];
                    mNumActiveObjects--;
                    break;
                }
            }
        }
        // we're entering the sweep space of an object
        else
        {
            // check for collisions with all active objects
            for ( unsigned int j = 0; j < mNumActiveObjects; ++j )
            {
                // if there's a collision
                if ( mSweepData[i].mObject->HasCollision( mActiveObjects[j] ) )
                {
                    // respond
                    mSweepData[i].mObject->SetColliding( true );
                    mActiveObjects[j]->SetColliding( true );
                }
            }

            // add to active list
            mActiveObjects[mNumActiveObjects++] = mSweepData[i].mObject;
        }
    }

}   // End of ObjectDB::Update()


//-------------------------------------------------------------------------------
// @ ObjectDB::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
ObjectDB::Render()                                  
{   
    // render each object
    unsigned int i;
    for ( i = 0; i < mNumObjects; ++i )
    {
        mObjects[i]->Render();
    }

    // render the extents
    ::IvSetWorldIdentity();
    for ( i = 0; i < 2*mNumObjects; ++i )
    {

        IvVector3 point1( mSweepData[i].mXValue, 1.0f, -1.0f );
        IvVector3 point2( mSweepData[i].mXValue, -1.0f, -1.0f );
        if ( mSweepData[i].mFinish )
            IvDrawLine( point1, point2, kCyan );
        else
            IvDrawLine( point1, point2, kMagenta );
    }

}   // End of ObjectDB::Render()


