//===============================================================================
// @ Game.h
// ------------------------------------------------------------------------------
// Game core
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#ifndef __GameDefs__
#define __GameDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvGame.h>
#include <IvTypes.h>

#include <IvVector3.h>
#include "IvClipper.h"

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvVector3;

class Game : public IvGame
{
public:
    Game();
    ~Game();
    bool PostRendererInitialize();

protected:
    virtual void UpdateObjects( float dt );
    virtual void Render();

    IvVector3 mPosition;
    IvClipper mClipper;
    IvVertexBuffer* mPlaneBuffer;        
    
private:
    Game( const Game& other );
    Game& operator=( const Game& other );

};

#endif
