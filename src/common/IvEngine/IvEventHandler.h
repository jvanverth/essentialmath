//===============================================================================
// @ IvEventHandler.h
// 
// Class to handle and store event input
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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

    void OnKeyDown( unsigned char key );  // Handle key down event
    void OnKeyUp( unsigned char key );    // Handle key up event
    void OnMouseDown( unsigned int h, unsigned int v );   // Handle mouse down event
    void OnMouseUp();                     // Handle mouse up event

    inline bool IsKeyDown( unsigned char key )      { return mKeys[key]; }
    inline bool IsKeyUp( unsigned char key )        { return !mKeys[key]; }

    bool IsKeyPressed( unsigned char key )
    {
        return mKeyPressed[key];
    }
    
    bool IsKeyReleased( unsigned char key )
    {
        return mKeyReleased[key];
    }
    
    inline bool IsMouseDown( unsigned int& h, unsigned int& v )
    {
        if (mMouseDown)
        {
            h = mMouseX; v = mMouseY;
            return true;
        }
        return false;
    }
    
    inline bool IsMousePressed( unsigned int& h, unsigned int& v )
    {
        if (mMousePressed)
        {
            h = mMouseX; v = mMouseY;
            return true;
        }
        return false;
    }

    inline bool IsMouseReleased()
    {
        return mMouseReleased;
    }
    
    inline void Update()
    {
        memset( mKeyPressed, 0, sizeof(bool)*256 );
        memset( mKeyReleased, 0, sizeof(bool)*256 );
        mMousePressed = false;
        mMouseReleased = false;
    }
    
private:
    bool mKeys[256];            // Array Used For The Keyboard Routine
    bool mKeyPressed[256];
    bool mKeyReleased[256];

    bool mMouseDown;
    bool mMousePressed;
    bool mMouseReleased;
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
