//===============================================================================
// @ IvMainDX11.cpp
// 
// Start up and main event loop for DX11
// ------------------------------------------------------------------------------
// Copyright (C) 2014 by James M. Van Verth. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//#pragma warning( disable : 4995 )
#include <windows.h>
#include <d3d11_1.h>
#include <directxcolors.h>

#include <stdlib.h>
#include <assert.h>

#include <IvGame.h>
#include <IvDebugger.h>
#include <IvEventHandler.h>
#include <DX11/IvRendererDX11.h>

HINSTANCE               gHInstance = 0;
HWND                    gHwnd = 0;

PCHAR*  CommandLineWToArgvA( PWCHAR CmdLine, int* _argc );

/*void    CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* userContext);
void    CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown );
void	CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
                         bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
                         int xPos, int yPos );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS11* pCaps );
bool	CALLBACK OnDeviceRemoved();

bool    CALLBACK IsDeviceAcceptable( D3DCAPS11* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
*/

bool    InitWindow(LPWSTR name, int& width, int& height, bool fullScreen = false);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

	int argc = 0;
	LPSTR *argv = CommandLineWToArgvA( lpCmdLine, &argc );

     // Perform any application-level initialization here
    gDebugger.DumpToFile( "Debug.txt" );

	if (!IvGame::Create() || !IvGame::mGame->PreRendererInitialize( argc, argv ))
	{
		IvGame::Destroy();
		return 1;
	}

    //DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
	int width = 640;
	int height = 480;
	if (!InitWindow(L"Example", width, height))
	{
		return 1;
	}

	// set up renderer
	if (!IvRendererDX11::Create(gHwnd) || !IvRendererDX11::mRenderer->Initialize(640, 480))
	{
		IvRenderer::Destroy();
		IvGame::Destroy();
		return 1;
	}

	// Do post-renderer creation initialization
	if (!IvGame::mGame->PostRendererInitialize())
	{
		IvRenderer::Destroy();
		IvGame::Destroy();
		return 1;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//*** Render();
		}
	}

    // Perform any application-level cleanup here
	//*** shutdown renderer/DX11
	IvRenderer::Destroy();
	IvGame::Destroy();

	return (int)msg.wParam;
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

bool InitWindow(LPWSTR name, int& width, int& height, bool fullScreen)
{
	gHInstance = GetModuleHandle(NULL);

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = gHInstance;
	wcex.hIcon = LoadIcon(gHInstance, (LPCTSTR)IDI_WINLOGO);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"ExampleWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
	if (!RegisterClassEx(&wcex))
		return false;

	// Adjust for fullscreen
	int posX, posY;
	if (fullScreen)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE dmScreenSettings;
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)width;
		dmScreenSettings.dmPelsHeight = (unsigned long)height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		posX = posY = CW_USEDEFAULT;

		RECT rc = { 0, 0, width, height };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
	}

	// Create window
	gHwnd = CreateWindow(L"ExampleWindowClass", name, WS_OVERLAPPEDWINDOW,
		posX, posY, width, height, NULL, NULL, gHInstance, NULL);
	if (!gHwnd)
	{
		return false;
	}

	ShowWindow(gHwnd, SW_SHOW);

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_ACTIVATE:
		//*** IvRenderer::mRenderer->Activate(wParam != WA_INACTIVE);
		break;

	case WM_SIZE:
		//*** IvRenderer::mRenderer->Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
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
	//*** IvRenderer::Destroy();
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
*/

//-------------------------------------------------------------------------------
// @ GetTime()
//-------------------------------------------------------------------------------
// Get current system time in milliseconds
//-------------------------------------------------------------------------------
unsigned int
GetTime()
{
	LARGE_INTEGER qwTime;
    QueryPerformanceCounter(&qwTime);

	LARGE_INTEGER qwTicksPerSec = { 0 };
	QueryPerformanceFrequency(&qwTicksPerSec);
	LONGLONG ticksPerSec = qwTicksPerSec.QuadPart;

	double fAppTime = (double)(qwTime.QuadPart) / (double)ticksPerSec;

	return (unsigned int)(fAppTime*1000.0);
}


//-------------------------------------------------------------------------------
// @ SetWindowTitle()
//-------------------------------------------------------------------------------
// Set the current window title to the string value
//-------------------------------------------------------------------------------
void
SetWindowTitle( const char* title )
{
    SetWindowTextA( gHwnd, title );
}

