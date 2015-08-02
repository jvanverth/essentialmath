//===============================================================================
// @ IvDebugger.cpp
// 
// Convenience class to dump debug information
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
//
//
// Use DEBUG_OUT combined with operator<< to dump debug text
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#elif __APPLE__ && __MACH__ && !NDEBUG
#include <assert.h>
#include <sys/sysctl.h>
#endif

#include "IvDebugger.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

IvDebugger gDebugger;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvDebugger::IvDebugger()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvDebugger::IvDebugger()
{
}   // End of IvDebugger::IvDebugger()


//-------------------------------------------------------------------------------
// @ IvDebugger::~IvDebugger()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvDebugger::~IvDebugger()
{
    if (mOutFile.is_open())
        mOutFile.close();

}   // End of IvDebugger::~IvDebugger()

//-------------------------------------------------------------------------------
// @ IvDebugger::DumpToFile()
//-------------------------------------------------------------------------------
// Set file to save debug info to
//-------------------------------------------------------------------------------
void
IvDebugger::DumpToFile( const char* string )
{
    if (mOutFile.is_open())
        mOutFile.close();

    // If a NULL filename is passed in, return without trying to open a file
    if (!string || string[0] == '\0')
        return;

    mOutFile.open(string, ios_base::trunc);

}   // End of IvDebugger::Put()

#if __APPLE__ && __MACH__
extern "C" {

#ifndef NDEBUG
//-------------------------------------------------------------------------------
// @ AmIBeingDebugged()
//-------------------------------------------------------------------------------
// From Mac Developer Library
//-------------------------------------------------------------------------------
static bool AmIBeingDebugged(void)
// Returns true if the current process is being debugged (either
// running under the debugger or has a debugger attached post facto).
{
    int                 junk;
    int                 mib[4];
    struct kinfo_proc   info;
    size_t              size;
    
    // Initialize the flags so that, if sysctl fails for some bizarre
    // reason, we get a predictable result.
    
    info.kp_proc.p_flag = 0;
    
    // Initialize mib, which tells sysctl the info we want, in this case
    // we're looking for information about a specific process ID.
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();
    
    // Call sysctl.
    
    size = sizeof(info);
    junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    assert(junk == 0);
    
    // We're being debugged if the P_TRACED flag is set.
    
    return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
}

//-------------------------------------------------------------------------------
// @ OutputDebugString()
//-------------------------------------------------------------------------------
// Courtesy of Jason Coco at
// http://stackoverflow.com/questions/417745/os-x-equivalent-to-outputdebugstring
//-------------------------------------------------------------------------------
void OutputDebugString(const char* fmt, ...)
{
    if( !AmIBeingDebugged() )
        return;
    
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#else
void OutputDebugString(const char* fmt, ...)
{
}
#endif
    
}
#endif


//-------------------------------------------------------------------------------
// @ IvDebugger::Flush()
//-------------------------------------------------------------------------------
// Flush debugger info
//-------------------------------------------------------------------------------
void
IvDebugger::Flush( void )
{
    // Get the buffer    
    std::string strText =  str();

    const char* cString = strText.c_str();

    // Dump to screen
#ifdef WIN32
    OutputDebugStringA(cString);
#elif __APPLE__ && __MACH__
    OutputDebugString(cString);
#endif

    // Write to output file, if open
    if (mOutFile.is_open())
    {
        mOutFile << cString;
        mOutFile.flush();
    }

    // reset the buffer for the next debugging info
    str("");

}   // End of IvDebugger::Flush()

