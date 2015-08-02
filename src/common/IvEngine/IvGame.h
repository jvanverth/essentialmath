//===============================================================================
// @ IvGame.h
// 
// Base game engine class
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//
//
// Usage notes:
//
// The static function Create() is declared here, but defined in the file 
// Game.cpp for a particular example.  This is to allow IvMain.cpp to just call
// IvGame::Create() to transparently create the particular class Game. 
//===============================================================================

#ifndef __IvGame__h__
#define __IvGame__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvStackAllocator.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvClock;
class IvEventHandler;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvGame
{
public:
    // Create needs to be implemented in the derived Game class
    static bool Create();

    static void Destroy();

    virtual bool PreRendererInitialize( int argc, char* argv[] );
    virtual bool PostRendererInitialize();
   
    void Update();
    void Display();
    inline void Quit()          { mQuit = true; }
    inline bool IsRunning()     { return !mQuit; }
    
    static IvGame*  mGame;          // global pointer

    IvClock*        mClock;         // main clock
    IvEventHandler* mEventHandler;  // event handling
    
protected:
    // constructor/destructor
    IvGame();
    virtual ~IvGame();

    bool ParseCommandLine( int argc, char* argv[] );
    bool SetupSubsystems();
    
    virtual void UpdateObjects( float dt ) = 0;
    virtual void Render() = 0;
    
    bool mQuit;
    bool mPaused;
        
private:
    // copy operations
    IvGame(const IvGame& other);
    IvGame& operator=(const IvGame& other);
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

extern void SetWindowTitle( const char* title );

#endif
