//===============================================================================
// @ IvMain.cpp
// 
// Start up and main event loop
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#ifdef PLATFORM_OSX
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <IvGame.h>
#include <IvDebugger.h>
#include <IvEventHandler.h>
#include <OGL/IvRendererOGL.h>

extern "C" void displayCallback(void);
extern "C" void visibilityCallback(int vis);
extern "C" void reshapeCallback(int w, int h);
extern "C" void keyboardCallback(unsigned char key, int x, int y);
extern "C" void keyboardupCallback(unsigned char key, int x, int y);
extern "C" void mouseCallback(int button, int state, int x, int y);
extern "C" void idleCallback(void);

//-------------------------------------------------------------------------------
// @ main()
//-------------------------------------------------------------------------------
// Where it all begins
//-------------------------------------------------------------------------------
int main(int argc, char *argv[])
{   
    glutInit( &argc, argv );
    gDebugger.DumpToFile( "Debug.txt" );

    // set up display
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    bool fullscreen = false;
    if (fullscreen)
    {
        glutGameModeString("640x480:16@60");
        glutEnterGameMode();
    }
    else
    {
        glutInitWindowSize(640, 480);
        glutCreateWindow("Example");
    }
#ifndef PLATFORM_OSX
	GLenum err = glewInit();
    if ( GLEW_OK != err )
    {
        ERROR_OUT("Error: " << glewGetErrorString(err) << std::endl);
        return 1;
    }

    // check for version compatibility
    if ( !GLEW_VERSION_2_0 )
    {
        ERROR_OUT("Error: OpenGL version 2.0 is not supported." << std::endl);
        return 1;
    }

    // check for ARB compatibility
    if ( !GLEW_ARB_shader_objects || !GLEW_ARB_fragment_shader || !GLEW_ARB_vertex_shader )
    {
        ERROR_OUT("Error: GLSL is not supported." << std::endl);
        return 1;
    }

    DEBUG_OUT("Using GLEW " << glewGetString(GLEW_VERSION));
#endif

    // set up game
    if (!IvGame::Create() || !IvGame::mGame->PreRendererInitialize( argc, argv ))
    {
        IvGame::Destroy();
        return 1;
    }

	// set up renderer
	if (!IvRendererOGL::Create() || !IvRendererOGL::mRenderer->Initialize( 640, 480 ))
    {
        IvRenderer::Destroy();
        IvGame::Destroy();
        return 1;
    }

    // Set the background black 
	IvRenderer::mRenderer->SetClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Do post-renderer creation initialization
    if (!IvGame::mGame->PostRendererInitialize())
    {
        IvRenderer::Destroy();
        IvGame::Destroy();
        return 1;
    }

    // set up display callbacks
    glutDisplayFunc(displayCallback);
    glutVisibilityFunc(visibilityCallback);
    glutReshapeFunc(reshapeCallback);

    // set up event handler
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardupCallback);
    glutMouseFunc(mouseCallback);

    // set up game update
    glutIdleFunc(idleCallback);

    // run it!
    glutMainLoop();
    
    IvGame::Destroy();

    return 0;
    
}   // End of main()


extern "C"
void
displayCallback(void)
{
    // clear the back buffer
	IvRenderer::mRenderer->ClearBuffers(kColorDepthClear);

    IvGame::mGame->Display();
}


extern "C"
void
visibilityCallback(int vis)
{
	IvRenderer::mRenderer->Activate( vis == GLUT_VISIBLE );
}


extern "C"
void
reshapeCallback(int w, int h)
{
    IvRenderer::mRenderer->Resize( w, h );
}


extern "C"
void
keyboardCallback(unsigned char key, int, int)
{
    IvGame::mGame->mEventHandler->KeyDown( key );   // Handle key down event
}


extern "C"
void
keyboardupCallback(unsigned char key, int, int)
{
    IvGame::mGame->mEventHandler->KeyUp( key ); // Handle key up event
}


extern "C"
void mouseCallback(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON )
    {
        if ( state == GLUT_DOWN )
            IvGame::mGame->mEventHandler->MouseDown( x, y );
        else
            IvGame::mGame->mEventHandler->MouseUp();
    }
}


extern "C"
void
idleCallback(void)
{
    // game does everything it needs to do
    IvGame::mGame->Update();

    // clear the back buffer
	IvRenderer::mRenderer->ClearBuffers(kColorDepthClear);

    // render the world
	if ( IvRenderer::mRenderer->IsActive() )
    {
        IvGame::mGame->Display();
    }
  
    // swap graphics buffers
    glutSwapBuffers();
}

//-------------------------------------------------------------------------------
// @ GetTime()
//-------------------------------------------------------------------------------
// Get current system time in milliseconds
//-------------------------------------------------------------------------------
unsigned int
GetTime()
{
    return glutGet(GLUT_ELAPSED_TIME);
}


//-------------------------------------------------------------------------------
// @ SetWindowTitle()
//-------------------------------------------------------------------------------
// Set the current window title to the string value
//-------------------------------------------------------------------------------
void
SetWindowTitle( const char* title )
{
    glutSetWindowTitle( title );
}


