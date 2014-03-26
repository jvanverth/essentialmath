//===============================================================================
// @ ObjectDB.h
// ------------------------------------------------------------------------------
// Object management class
//
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// To use this, you need to call Initialize() with the maximum number of objects
// you expect to see.  Then you can add new objects using AddObject().  Calling
// Update() will move the objects and update their collision status.
//===============================================================================

#ifndef __ObjectDBDefs__
#define __ObjectDBDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class CollisionObject;

// data structure for holding sweep point info
struct SweepPoint
{
    float               mXValue;    // extent value
    CollisionObject*    mObject;    // the object associated with this extent
    bool                mFinish;    // whether this is a start or finish endpoint
};

// main class
class ObjectDB
{
public:
    ObjectDB();
    ~ObjectDB();

    bool Initialize( unsigned int maxObjects );
    bool AddObject( CollisionObject* object );
    void Clean();

    void Update( float dt );
    void Render();                              

private:
    unsigned int        mNumObjects;            // number of objects in database
    unsigned int        mMaxObjects;            // maximum number we can have
    CollisionObject**   mObjects;               // the array of object pointers
    SweepPoint*         mSweepData;             // array of sweep-and-prune data
    CollisionObject**   mActiveObjects;         // active list of potential colliders
    unsigned int        mNumActiveObjects;      // number of members in active list
};

#endif
