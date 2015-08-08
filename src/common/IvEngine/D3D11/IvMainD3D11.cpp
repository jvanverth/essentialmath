//===============================================================================
// @ IvMainD3D11.cpp
// 
// Start up and main event loop for D3D11
// ------------------------------------------------------------------------------
// Copyright (C) 2014 by James M. Van Verth. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

//#pragma warning( disable : 4995 )
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <directxcolors.h>
#include <VersionHelpers.h>

#include <stdlib.h>
#include <assert.h>

#include <IvGame.h>
#include <IvDebugger.h>
#include <IvEventHandler.h>
#include <D3D11/IvRendererD3D11.h>

HINSTANCE               gHInstance = nullptr;
HWND                    gHwnd = nullptr;
IDXGISwapChain*         gSwapChain = nullptr;
ID3D11Device*           gDevice = nullptr;
ID3D11DeviceContext*    gContext = nullptr;
ID3D11RenderTargetView* gRenderTargetView = nullptr;
ID3D11Texture2D*        gDepthStencilBuffer = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;
UINT                    gSyncInterval = 0;
ID3D11Debug*            gD3dDebug;
bool                    gIsFullscreen = false;

PCHAR*  CommandLineWToArgvA( PWCHAR CmdLine, int* _argc );

void    CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown );
void    CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
                         bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
                         int xPos, int yPos );
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
    bool fullscreen = false;
    if (!InitWindow(L"Example", width, height, fullscreen))
    {
        IvGame::Destroy();
        return 1;
    }

    if (!InitDevice(width, height, fullscreen, fullscreen))
    {
        DestroyDevice();
        IvGame::Destroy();
        return 1;
    }

    ShowWindow(gHwnd, SW_SHOW);

    // set up renderer
    if (!IvRendererD3D11::Create(gDevice, gContext, gRenderTargetView, gDepthStencilView)
        || !IvRendererD3D11::mRenderer->Initialize(width, height))
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
            if (IvRenderer::mRenderer->IsActive())
            {
                IvGame::mGame->Display();
            }

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
    gHInstance = GetModuleHandle(nullptr);

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
    gIsFullscreen = fullscreen;

    // Create window
    gHwnd = CreateWindow(L"ExampleWindowClass", name, WS_OVERLAPPEDWINDOW,
                         posX, posY, width, height, nullptr, nullptr, gHInstance, nullptr);
    if (!gHwnd)
    {
        return false;
    }

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
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
    HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0,
                                           D3D11_SDK_VERSION, &swapChainDesc, &gSwapChain, 
                                           &gDevice, nullptr, &gContext);
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
    
    return true;
}

bool GetRefreshRate(unsigned int width, unsigned int height, 
                    int& numerator, int& denominator)
{
    IDXGIFactory* factory;

    HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
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

    // Now we enumerate all the display modes that match 32-bit RGBA w/sRGB gamma
    unsigned int numModes;
    DXGI_MODE_DESC* displayModeList = nullptr;
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
    if (SUCCEEDED(result))
    {
        displayModeList = new DXGI_MODE_DESC[numModes];
        if (nullptr != displayModeList)
        {
            result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
        }
    }
    if (nullptr == displayModeList || FAILED(result))
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
    //  return false;
    //}

    //// Store the dedicated video card memory in megabytes.
    //m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    //// Convert the name of the video card to a character array and store it.
    //unsigned int stringLength;
    //error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    //if (error != 0)
    //{
    //  return false;
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
        gSwapChain->SetFullscreenState(false, nullptr);
    }
    
    if (gDepthStencilView)
    {
        gDepthStencilView->Release();
        gDepthStencilView = nullptr;
    }

    if (gDepthStencilBuffer)
    {
        gDepthStencilBuffer->Release();
        gDepthStencilBuffer = nullptr;
    }
    
    if (gRenderTargetView)
    {
        gRenderTargetView->Release();
        gRenderTargetView = nullptr;
    }

    if (gSwapChain)
    {
        gSwapChain->Release();
        gSwapChain = nullptr;
    }

    if (gContext)
    {
        gContext->Release();
        gContext = nullptr;
    }
    
#if _DEBUG
//  gD3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    if (gD3dDebug)
    {
        gD3dDebug->Release();
        gD3dDebug = nullptr;
    }
#endif

    if (gDevice)
    {
        gDevice->Release();
        gDevice = nullptr;
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
        if (IvRenderer::mRenderer)
        {
            IvRenderer::mRenderer->Activate(wParam != WA_INACTIVE);
        }
        break;

    case WM_SIZE:
        if (gSwapChain)
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            gContext->OMSetRenderTargets(0, 0, 0);

            // Release all outstanding references to the swap chain's buffers.
            if (IvRenderer::mRenderer)
            {
                reinterpret_cast<IvRendererD3D11*>(IvRenderer::mRenderer)->ReleaseTargets();
            }
            if (gRenderTargetView)
            {
                gRenderTargetView->Release();
                gRenderTargetView = nullptr;
            }
            if (gDepthStencilView)
            {
                gDepthStencilView->Release();
                gDepthStencilView = nullptr;
            }
            if (gDepthStencilBuffer)
            {
                gDepthStencilBuffer->Release();
                gDepthStencilBuffer = nullptr;
            }

            HRESULT hr;
            // Preserve the existing buffer count and format.
            // Automatically choose the width and height to match the client rect for HWNDs.
            hr = gSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

            // Create a render target view
            ID3D11Texture2D* backBuffer = nullptr;
            hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
            if (FAILED(hr))
            {
                PostQuitMessage(0);
                return E_FAIL;
            }
            hr = gDevice->CreateRenderTargetView(backBuffer, nullptr, &gRenderTargetView);
            backBuffer->Release();
            if (FAILED(hr))
            {
                PostQuitMessage(0);
                return E_FAIL;
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
            hr = gDevice->CreateTexture2D(&depthBufferDesc, nullptr, &gDepthStencilBuffer);
            if (FAILED(hr))
            {
                gRenderTargetView->Release();
                gRenderTargetView = 0;
                PostQuitMessage(0);
                return E_FAIL;
            }

            // And now create the depth stencil view
            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
            ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
            depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depthStencilViewDesc.Texture2D.MipSlice = 0;
            hr = gDevice->CreateDepthStencilView(gDepthStencilBuffer, &depthStencilViewDesc, &gDepthStencilView);
            if (FAILED(hr))
            {
                gDepthStencilBuffer->Release();
                gDepthStencilBuffer = 0;
                gRenderTargetView->Release();
                gRenderTargetView = 0;
                PostQuitMessage(0);
                return E_FAIL;
            }

            // Bind the render target view and depth stencil buffer to the output render pipeline.
            gContext->OMSetRenderTargets(1, &gRenderTargetView, gDepthStencilView);
        }

        if (IvRenderer::mRenderer)
        {
            reinterpret_cast<IvRendererD3D11*>(IvRenderer::mRenderer)->SetTargets(gRenderTargetView, gDepthStencilView);
            IvRenderer::mRenderer->Resize(LOWORD(lParam), HIWORD(lParam));
        }
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

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        {
            bool bLeftDown = ((wParam & MK_LBUTTON) != 0);
            bool bRightDown = ((wParam & MK_RBUTTON) != 0);
            bool bMiddleDown = ((wParam & MK_MBUTTON) != 0);

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            if (!gIsFullscreen)
            {
                RECT rc = { 0, 0, 640, 480 };
                AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
                xPos -= rc.left;
                yPos -= rc.top;
            }

            OnMouse(bLeftDown, bRightDown, bMiddleDown, false, false, 0, xPos, yPos);
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
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
        int a = wctob(w);   // convert to char
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
    argv[argc] = nullptr;

    (*_argc) = argc;
    return argv;
}
