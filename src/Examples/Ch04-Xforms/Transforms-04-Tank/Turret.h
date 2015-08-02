//===============================================================================
// @ Turret.h
// ------------------------------------------------------------------------------
// Turret of player
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

#ifndef __TurretDefs__
#define __TurretDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix33.h>
#include <IvVector3.h>

class Barrel;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class Turret
{
public:
    Turret();
    ~Turret();

    void Update( float dt );
    void Render( float parentScale, 
                 const IvMatrix33& parentRotate, 
                 const IvVector3& parentTranslation );

    bool ReadData();

private:
    void CleanData();

    // transformation
    float       mScale;
    IvMatrix33  mRotate;
    IvVector3   mTranslate;

    // geometry
    IvVertexBuffer* mVertices;
    IvIndexBuffer*  mIndices;

    // children
    Barrel*     mBarrel;
    
};

#endif
