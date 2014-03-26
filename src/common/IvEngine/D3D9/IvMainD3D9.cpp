//===============================================================================
// @ IvMainD3D9.cpp
// 
// Start up and main event loop for D3D9
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#pragma warning( disable : 4995 )
#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <d3d9.h>
#include "DXUT.h"

#include <IvGame.h>
#include <IvDebugger.h>
#include <IvEventHandler.h>
#include <D3D9/IvRendererD3D9.h>

PCHAR*  CommandLineWToArgvA( PWCHAR CmdLine, int* _argc );

void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
void    CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown );
void	CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
                         bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
                         int xPos, int yPos );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );
bool	CALLBACK OnDeviceRemoved();

bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
void    CALLBACK OnLostDevice();
void    CALLBACK OnDestroyDevice();

//-------------------------------------------------------------------------------
// @ wWinMain()
//-------------------------------------------------------------------------------
// Where it all begins
//-------------------------------------------------------------------------------
int WINAPI 
wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMouse( OnMouse );
    DXUTSetCallbackMsgProc( MsgProc );

    // Set the D3D9 DXUT callbacks. Remove these sets if the app doesn't need to support D3D9
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );

    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params

	int argc = 0;
	LPSTR *argv = CommandLineWToArgvA( lpCmdLine, &argc );

     // Perform any application-level initialization here
    gDebugger.DumpToFile( "Debug.txt" );

	if (!IvGame::Create() || !IvGame::mGame->PreRendererInitialize( argc, argv ))
	{
		IvGame::Destroy();
		return 1;
	}

    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Example" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );  
    DXUTMainLoop(); // Enter into the DXUT render loop

    // Perform any application-level cleanup here
    IvGame::Destroy();

    return DXUTGetExitCode();
}

//
// CommandLineWToArgvA - converts Windows command line string to Unix-style command line arguments
//
// Based on code by Alexander A. Telyatnikov (http://alter.org.ua/en/docs/win/args/)
// 
PCHAR*
CommandLineWToArgvA( PWCHAR CmdLine, int* _argc )
{
    PCHAR* argv;
    PCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    WCHAR   w;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = wcslen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(CHAR));

    _argv = (PCHAR)(((PCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( w = CmdLine[i] ) {
		int a = wctob(w);	// convert to char
		assert( a >= 0 );
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}


//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed )
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback defined by this app, so reject any device that 
    // doesn't support at least ps2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps  )
{
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
	// set up renderer
	if (!IvRendererD3D9::Create( pd3dDevice ) || !IvRendererD3D9::mRenderer->Initialize( 640, 480 ))
        return false;

    // set up game stuff that depends on renderer
    IvGame::mGame->PostRendererInitialize();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    // clear the back buffer
	IvRenderer::mRenderer->ClearBuffers(kColorDepthClear);

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
	    IvGame::mGame->Display();
        (void) pd3dDevice->EndScene();
    }
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    // game move objects and the like
    IvGame::mGame->Update();
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice()
{
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
	// delete the renderer
	IvRenderer::Destroy();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing )
{
	if ( WM_ACTIVATE == uMsg )
	{
		IvRenderer::mRenderer->Activate( wParam != WA_INACTIVE );
	}
	else if ( WM_SIZE == uMsg )
	{
	    IvRenderer::mRenderer->Resize( LOWORD(lParam), HIWORD(lParam) );
	}
	else if ( WM_PAINT == uMsg )
	{
		
	}

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown )
{
    UINT theChar = MapVirtualKey( nChar, 2 );
	if ( bKeyDown)
	{
		IvGame::mGame->mEventHandler->KeyDown( theChar );   // Handle key down event
	}
	else
	{
		IvGame::mGame->mEventHandler->KeyUp( theChar ); // Handle key up event
	}
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
                       int xPos, int yPos )
{
    if ( bLeftButtonDown )
	{
        IvGame::mGame->mEventHandler->MouseDown( xPos, yPos );
	}
    else
	{
        IvGame::mGame->mEventHandler->MouseUp();
    }
}


//-------------------------------------------------------------------------------
// @ GetTime()
//-------------------------------------------------------------------------------
// Get current system time in milliseconds
//-------------------------------------------------------------------------------
unsigned int
GetTime()
{
    return (unsigned int)(DXUTGetTime()*1000.0);
}


//-------------------------------------------------------------------------------
// @ SetWindowTitle()
//-------------------------------------------------------------------------------
// Set the current window title to the string value
//-------------------------------------------------------------------------------
void
SetWindowTitle( const char* title )
{
    SetWindowTextA( DXUTGetHWND(), title );
}


