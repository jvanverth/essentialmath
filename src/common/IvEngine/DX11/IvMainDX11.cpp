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
#include <d3d11.h>
#include <dxgi1_3.h>
#include <directxcolors.h>

#include <stdlib.h>
#include <assert.h>

#include <IvGame.h>
#include <IvDebugger.h>
#include <IvEventHandler.h>
#include <DX11/IvRendererDX11.h>

HINSTANCE               gHInstance = NULL;
HWND                    gHwnd = NULL;
IDXGISwapChain*         gSwapChain = NULL;
ID3D11Device*			gDevice = NULL;
ID3D11DeviceContext*    gContext = NULL;
ID3D11RenderTargetView* gRenderTargetView = NULL;
ID3D11Texture2D*        gDepthStencilBuffer = NULL;
ID3D11DepthStencilView* gDepthStencilView = NULL;
UINT					gSyncInterval = 0;
ID3D11Debug*            gD3dDebug;

PCHAR*  CommandLineWToArgvA( PWCHAR CmdLine, int* _argc );

//void    CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* userContext);
void    CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown );
void	CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
                         bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
                         int xPos, int yPos );
/*LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS11* pCaps );
bool	CALLBACK OnDeviceRemoved();

bool    CALLBACK IsDeviceAcceptable( D3DCAPS11* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
*/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool    InitWindow(LPWSTR name, int& width, int& height, bool fullScreen = false);
bool    InitDevice(unsigned int width, unsigned int height, 
	               bool fullscreen = false, bool vsync = false);
void    DestroyDevice();
bool    GetRefreshRate(unsigned int width, unsigned int height, 
	                   int& numerator, int& denominator);

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
		IvGame::Destroy();
		return 1;
	}

	if (!InitDevice(width, height))
	{
		DestroyDevice();
		IvGame::Destroy();
		return 1;
	}

	// set up renderer
	if (!IvRendererDX11::Create(gDevice, gContext, gRenderTargetView, gDepthStencilView)
		|| !IvRendererDX11::mRenderer->Initialize(width, height))
	{
		IvRenderer::Destroy();
		DestroyDevice();
		IvGame::Destroy();
		return 1;
	}

	// Do post-renderer creation initialization
	if (!IvGame::mGame->PostRendererInitialize())
	{
		IvGame::Destroy();
		DestroyDevice();
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
			// game move objects and the like
			IvGame::mGame->Update();

			// clear the back buffer
			IvRenderer::mRenderer->ClearBuffers(kColorDepthClear);

			// Render the scene
			IvGame::mGame->Display();

			// swap buffers
			gSwapChain->Present(gSyncInterval, 0);
		}
	}
	
    // Perform any application-level cleanup here

	IvGame::Destroy();
	DestroyDevice();

	return (int)msg.wParam;
}

bool InitWindow(LPWSTR name, int& width, int& height, bool fullscreen)
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
	{
		return false;
	}

	int posX, posY;
	if (fullscreen)
	{
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
		RECT rc = { 0, 0, width, height };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;

		posX = posY = CW_USEDEFAULT;
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

bool InitDevice(unsigned int width, unsigned int height, bool fullscreen, bool vsync)
{
	int numerator, denominator;
	if (vsync)
	{
		if (!GetRefreshRate(width, height, numerator, denominator))
		{
			return false;
		}
		gSyncInterval = 1;
	}
	else
	{
		numerator = 0;
		denominator = 1;
		gSyncInterval = 0;
	}

	// Set up the swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;                                  // one backbuffer
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = gHwnd;
	swapChainDesc.SampleDesc.Count = 1;       
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !fullscreen;

#if _DEBUG
	UINT deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT deviceFlags = 0;
#endif
	// Create the swap chain, Direct3D device, and Direct3D device context.
	HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, NULL, 0,
		                                   D3D11_SDK_VERSION, &swapChainDesc, &gSwapChain, 
										   &gDevice, NULL, &gContext);
	if (FAILED(result))
	{
		//*** fallback to WARP?
		return false;
	}
	
#if _DEBUG
	if (FAILED(gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&gD3dDebug))))
	{
		return false;
	}
#endif

	// Create a render target view
	ID3D11Texture2D* backBuffer = NULL;
	HRESULT hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
	{
		return false;
	}
	hr = gDevice->CreateRenderTargetView(backBuffer, NULL, &gRenderTargetView);
	backBuffer->Release();
	if (FAILED(hr))
	{
		return false;
	}
	
	// Create depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	result = gDevice->CreateTexture2D(&depthBufferDesc, NULL, &gDepthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// And now create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = gDevice->CreateDepthStencilView(gDepthStencilBuffer, &depthStencilViewDesc, &gDepthStencilView);
	if (FAILED(result))
	{
		return false;
	}
	
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	gContext->OMSetRenderTargets(1, &gRenderTargetView, gDepthStencilView);
	
	return true;
}

bool GetRefreshRate(unsigned int width, unsigned int height, 
	                int& numerator, int& denominator)
{
	IDXGIFactory* factory;
#if _DEBUG
	UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT flags = 0;
#endif
	HRESULT result = CreateDXGIFactory2(flags, __uuidof(IDXGIFactory2), (void**)&factory);
	if (FAILED(result))
	{
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	}
	if (FAILED(result))
	{
		return false;
	}

	// We start by getting the primary graphics interface, and from that getting the primary output 
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	result = factory->EnumAdapters(0, &adapter);
	if (SUCCEEDED(result))
	{
		result = adapter->EnumOutputs(0, &adapterOutput);
	} 
	else
	{
		adapter->Release();
		factory->Release();
		return false;
	}

	// Now we enumerate all the display modes that match 32-bit RGBA
	unsigned int numModes;
	DXGI_MODE_DESC* displayModeList = NULL;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (SUCCEEDED(result))
	{
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (NULL != displayModeList)
		{
			result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		}
	}
	if (NULL == displayModeList || FAILED(result))
	{
		adapterOutput->Release();
		adapter->Release();
		factory->Release();
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (unsigned int i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == width)
		{
			if (displayModeList[i].Height == height)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	//DXGI_ADAPTER_DESC adapterDesc;
	//result = adapter->GetDesc(&adapterDesc);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// Store the dedicated video card memory in megabytes.
	//m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//// Convert the name of the video card to a character array and store it.
	//unsigned int stringLength;
	//error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	//if (error != 0)
	//{
	//	return false;
	//}

	// Clean up
	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	return true;
}

void DestroyDevice()
{
	if (gSwapChain)
	{
		gSwapChain->SetFullscreenState(false, NULL);
	}
	
	if (gDepthStencilView)
	{
		gDepthStencilView->Release();
		gDepthStencilView = NULL;
	}

	if (gDepthStencilBuffer)
	{
		gDepthStencilBuffer->Release();
		gDepthStencilBuffer = NULL;
	}
	
	if (gRenderTargetView)
	{
		gRenderTargetView->Release();
		gRenderTargetView = NULL;
	}

	if (gContext)
	{
		gContext->Release();
		gContext = NULL;
	}
	
#if _DEBUG
//	gD3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	if (gD3dDebug)
	{
		gD3dDebug->Release();
		gD3dDebug = NULL;
	}
#endif

	if (gDevice)
	{
		gDevice->Release();
		gDevice = NULL;
	}

	if (gSwapChain)
	{
		gSwapChain->Release();
		gSwapChain = NULL;
	}
	
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

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			DWORD dwMask = (1 << 29);
			bool bAltDown = ((lParam & dwMask) != 0);

			OnKeyboard((UINT)wParam, true, bAltDown);
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			DWORD dwMask = (1 << 29);
			bool bAltDown = ((lParam & dwMask) != 0);

			OnKeyboard((UINT)wParam, false, bAltDown);
		}
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
*/

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

//
// CommandLineWToArgvA - converts Windows command line string to Unix-style command line arguments
//
// Based on code by Alexander A. Telyatnikov (http://alter.org.ua/en/docs/win/args/)
// 
PCHAR*
CommandLineWToArgvA(PWCHAR CmdLine, int* _argc)
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
	i = ((len + 2) / 2)*sizeof(PVOID)+sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2)*sizeof(CHAR));

	_argv = (PCHAR)(((PCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (w = CmdLine[i]) {
		int a = wctob(w);	// convert to char
		assert(a >= 0);
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			}
			else {
				_argv[j] = a;
				j++;
			}
		}
		else {
			switch (a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
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
