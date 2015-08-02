//===============================================================================
// @ IvEventHandler.h
// 
// Class to handle and store event input
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//===============================================================================

#ifndef __IvEventHandler__h__
#define __IvEventHandler__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "IvWriter.h"

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class IvEventHandler;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvEventHandler
{
public:
    // constructor/destructor
    IvEventHandler();
    ~IvEventHandler();
    
    // text output (for debugging)
    friend IvWriter& operator<<( IvWriter& out, const IvEventHandler& source );

    void KeyDown( unsigned char key );  // Handle key down event
    void KeyUp( unsigned char key );    // Handle key up event
    void MouseDown( unsigned int h, unsigned int v );   // Handle mouse down event
    void MouseUp();                     // Handle mouse up event

    inline bool IsKeyDown( unsigned char key )      { return mKeys[key]; }
    
    inline bool IsMouseDown( unsigned int& h, unsigned int& v )
    {
        if (mMouseDown)
        {
            h = mMouseX; v = mMouseY;
            return true;
        }
        return false;
    }

private:

    bool mKeys[256];            // Array Used For The Keyboard Routine
    bool mMouseDown;
    unsigned int mMouseX;
    unsigned int mMouseY;

    // copy operations
    IvEventHandler(const IvEventHandler& other);
    IvEventHandler& operator=(const IvEventHandler& other);
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

#endif
