//===============================================================================
// @ IvEventHandler.cpp
// 
// Event handling routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvEventHandler.h"
#include <string.h>
#include <ctype.h>

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvEventHandler::IvEventHandler()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvEventHandler::IvEventHandler() 
{
    // clear key states
    memset(mKeys, 0, sizeof(bool)*256);
    mMouseX = 0;  mMouseY = 0;
    mMouseDown = false;
        
}   // End of IvEventHandler::IvEventHandler()

//-------------------------------------------------------------------------------
// @ IvEventHandler::~IvEventHandler()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvEventHandler::~IvEventHandler()
{
}   // End of IvEventHandler::~IvEventHandler()
    

//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
IvWriter& 
operator<<(IvWriter& out, const IvEventHandler& source)
{
    unsigned int i;
    for (i = 0; i < 256; i++)
    {
        if (source.mKeys[i])
            out << "(Key = " << i << ") ";
    }

    out << (source.mMouseDown ? " Mouse Down " : " Mouse Up ");
    out << source.mMouseX  << ' ' << source.mMouseY  << eol;

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ IvEventHandler::KeyDown()
//-------------------------------------------------------------------------------
// Handle key down event
//-------------------------------------------------------------------------------
void
IvEventHandler::KeyDown( unsigned char key )
{
    if (isupper(key))
        key = tolower(key);
    mKeys[key] = true;          // Mark It As true

}   // End of IvEventHandler::KeyDown()



//-------------------------------------------------------------------------------
// @ IvEventHandler::KeyUp()
//-------------------------------------------------------------------------------
// Handle key up event
//-------------------------------------------------------------------------------
void
IvEventHandler::KeyUp( unsigned char key )
{
    if (isupper(key))
        key = tolower(key);
    mKeys[key] = false;         // Mark It As false
    
}   // End of IvEventHandler::KeyUp()



//-------------------------------------------------------------------------------
// @ IvEventHandler::MouseDown()
//-------------------------------------------------------------------------------
// Handle mouse down event
//-------------------------------------------------------------------------------
void
IvEventHandler::MouseDown( unsigned int h, unsigned int v )
{
    mMouseDown = true;
    mMouseX = h;
    mMouseY = v;

}   // End of IvEventHandler::MouseDown()



//-------------------------------------------------------------------------------
// @ IvEventHandler::MouseUp()
//-------------------------------------------------------------------------------
// Handle mouse up event
//-------------------------------------------------------------------------------
void
IvEventHandler::MouseUp()
{
    mMouseDown = false;
    
}   // End of IvEventHandler::MouseUp()
