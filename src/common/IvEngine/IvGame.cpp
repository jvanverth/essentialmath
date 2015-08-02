//===============================================================================
// @ IvGame.cpp
// 
// Main game engine class
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include "IvGame.h"
#include "IvClock.h"
#include "IvEventHandler.h"
#include "IvRenderer.h"
#include "IvRendererHelp.h"

#include "IvDebugger.h"


//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

IvGame*          IvGame::mGame = 0;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvGame::IvGame()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvGame::IvGame() :
    mClock( 0 )
    ,mEventHandler( 0 )
    ,mPaused( false )
    ,mQuit( false )
{
    mGame = this;
}   // End of IvGame::IvGame()


//-------------------------------------------------------------------------------
// @ IvGame::~IvGame()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvGame::~IvGame()
{
    mGame = 0;
    
    if (mEventHandler)
    {
        delete mEventHandler;
        mEventHandler = 0;
    }

    IvCleanUpRendererHelpData();
    IvRenderer::Destroy();

    if (mClock)
    {
        delete mClock;
        mClock = 0;
    }

    if (IvStackAllocator::mScratchAllocator)
    {
        delete IvStackAllocator::mScratchAllocator;
        IvStackAllocator::mScratchAllocator = 0;
    }

}   // End of IvGame::~IvGame() 

//-------------------------------------------------------------------------------
// @ IvGame::Destroy()
//-------------------------------------------------------------------------------
// Shutdown function
//-------------------------------------------------------------------------------
void
IvGame::Destroy()
{
    delete mGame;
    
}   // End of IvGame::Initialize()
    

//-------------------------------------------------------------------------------
// @ IvGame::PreRendererInitialize()
//-------------------------------------------------------------------------------
// Main initializer
//-------------------------------------------------------------------------------
bool
IvGame::PreRendererInitialize( int argc, char* argv[] )
{
    // check command line arguments
    if ( !ParseCommandLine( argc, argv ) )
        return false;

    // set up initial subsystems
    if ( !SetupSubsystems() )
        return false;

    return true;
    
}   // End of IvGame::PreRendererInitialize()
    

//-------------------------------------------------------------------------------
// @ IvGame::PostRendererInitialize()
//-------------------------------------------------------------------------------
// Post renderer initializer
//-------------------------------------------------------------------------------
bool
IvGame::PostRendererInitialize()
{
    return true;
    
}   // End of IvGame::PostRendererInitialize()


//-------------------------------------------------------------------------------
// @ IvGame::ParseCommandLine()
//-------------------------------------------------------------------------------
// Parse command line arguments
//-------------------------------------------------------------------------------
bool 
IvGame::ParseCommandLine( int, char*[] )
{
    return true;
    
}   // End of IvGame::ParseCommandLine()


//-------------------------------------------------------------------------------
// @ IvGame::SetupSubsystems()
//-------------------------------------------------------------------------------
// Set up internal subsystems
//-------------------------------------------------------------------------------
bool 
IvGame::SetupSubsystems()
{
    // create the scratch allocator
    IvStackAllocator::mScratchAllocator = new IvStackAllocator(1 * 1024 * 1024);

    // create display
    mClock = new IvClock;
    if ( !mClock )
        return false;

    mClock->Update();

    // create event handler
    mEventHandler = new IvEventHandler;
    if (!mEventHandler)
        return false;

    return true;
    
}   // End of IvGame::SetupSubsystems()



//-------------------------------------------------------------------------------
// @ IvGame::Update()
//-------------------------------------------------------------------------------
// Run main update loop
//-------------------------------------------------------------------------------
void
IvGame::Update()
{
    IvStackAllocator::mScratchAllocator->Reset();

    if ( mQuit )
    {
        IvGame::Destroy();
        exit(0);
    }

    // if escape is hit
    if (mEventHandler->IsKeyDown(27))
        Quit();

    // update clock to next frame
    mClock->Update();

    // if sim is running, update objects
    if (!mPaused)
    {
        UpdateObjects(mClock->GetElapsedTime());
    }
    
}   // End of IvGame::Update()


//-------------------------------------------------------------------------------
// @ IvGame::Display()
//-------------------------------------------------------------------------------
// Display rendered data
//-------------------------------------------------------------------------------
void
IvGame::Display()
{
    // draw objects
    Render();
    
}   // End of IvGame::Display()
