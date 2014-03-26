//--------------------------------------------------------------------------------------
// File: DxStdAfx.h
//
// Desc: Header file that is the standard includes for the DirectX SDK samples
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXSDK_STDAFX_H
#define DXSDK_STDAFX_H

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define STRICT

// Works with Windows 2000 and later and Windows 98 or later
#undef _WIN32_IE
#undef WINVER
#undef _WIN32_WINDOWS
#undef _WIN32_WINNT
#define WINVER         0x0500 
#define _WIN32_WINDOWS 0x0410 
#define _WIN32_WINNT   0x0500 

#include <windows.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>      // for placement new
#include <math.h>      
#include <limits.h>      
#include <stdio.h>


// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#define D3D_DEBUG_INFO
#endif

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

// strsafe.h deprecates old unsecure string functions.  If you 
// really do not want to it to (not recommended), then uncomment the next line and 
// comment out the following lines:

//#define STRSAFE_NO_DEPRECATE
#pragma deprecated(strncpy)
#pragma deprecated(wcsncpy)
#pragma deprecated(_tcsncpy)
#pragma deprecated(wcsncat)
#pragma deprecated(strncat)
#pragma deprecated(_tcsncat)

#include <strsafe.h>

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTenum.h"
#include "DXUTeffectmap.h"
#include "DXUTmesh.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "DXUTSound.h"

#if defined(DEBUG) | defined(_DEBUG)
    #ifndef V
        #define V(x)           { hr = x; if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
    #endif
    #ifndef V_RETURN
        #define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
    #endif
#else
    #ifndef V
        #define V(x)           { hr = x; }
    #endif
    #ifndef V_RETURN
        #define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
    #endif
#endif

#ifndef SAFE_DELETE
    #define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#endif // !defined(DXSDK_STDAFX_H)
