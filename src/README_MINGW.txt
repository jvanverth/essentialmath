***********************************************************************
*** Windows (Minimal GNU for Windows) Platform-Specific README FILE ***

If you have not already done so, please read the all-platform introductory file, README_FIRST.TXT, which is located in the same directory as this file.


***********************************************************************
*** System Requirements / Installation ***

* This code distribution supports Windows 2000, Windows XP and Windows Vista

* The contents of the book's CDROM must be copied (keeping the hierarchy) to the local machine, as described in README_FIRST.TXT

* The file Win32System/glut32.DLL must be copied from this book's CDROM contents into:
	* Windows XP: /Windows/System32
	* Windows 2000: /Winnt/System32

* MinGW must be installed - see the section "MinGW Specifics" below for details

* MSYS must be installed - see the section "MSYS Specifics" below for details

* Before building examples you will need to create the necessary libraries.
See "Building Libraries and Demo Applications", for more details.


***********************************************************************
*** MinGW Specifics ***

The system must have MinGW installed.  MinGW may be downloaded from:
	http://www.mingw.org/download.shtml

This CDROM was built and tested using MinGW for Windows version 5.1.3

The MinGW/bin directory (wherever it may have been installed to your machine) must be added to your path.  See http://www.mingw.org/mingwfaq.shtml for details.


***********************************************************************
*** MSYS Specifics ***

The system must have MSYS (Minimal SYStem) installed.  MSYS may be downloaded from:
	http://www.mingw.org/download.shtml

This CDROM was built and tested using MinGW for Windows version 1.0.10.

The MSYS/1.0/bin directory (wherever it may have been installed to your machine) must be added to your path.  See http://www.mingw.org/mingwfaq.shtml for details.


***********************************************************************
*** Building Libraries and Demo Applications ***

The libraries and applications use the OpenGL GLUT system.  GLUT has been included on the CDROM in the top-level directory /gl.  All of the libraries and applications reference this directory via relative paths.  As a result, there should be no need to add any path to GLUT into your development tool settings.  However, see the note above regarding the GLUT runtime DLL.

The MinGW platform uses "make" from a Windows command prompt to build any or all of the libraries and demo applications.  A hierarchical build system allows the user to rebuild at any desired level in the code hierarchy.

To build, open a Windows command prompt and change directory to the desired directory:

* To build or rebuild all Iv libraries in a single command, change directories to /common and build as described below.

* To build or rebuild a specific Iv library (such as IvMath), change directories to the /common subdirectory for the library (e.g. /common/IvMath) and build as described below.

* The library makefiles copy that library's header files to /common/Includes at the end of compilation.  In addition, the library itself is built to the /common/Libs subdirectory that matches the build configuration (MinGWRelease or MinGWDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

* To rebuild all demo applications for all chapters in a single command, change directories to /Examples and build as described below.

* To rebuild all demo applications for a specific chapter in a single command (such as Chapter 4), change directories to that chapter's subdirectory under /Examples (e.g. /Examples/Ch04-Xforms) and build as described below.

* To rebuild a specific demo application, change directories to that demo's subdirectory under /Examples/Ch* and build as described below.

* To build, note that MinGW is NOT THE DEFAULT PLATFORM.  To build any part of the code for MinGW, use the following commands in the desired build directory:

* For release builds:

make PLATFORM=MinGW

OR

make release PLATFORM=MinGW

* For debugging builds:

make debug PLATFORM=MinGW

* To clear out all builds of the current directory for MinGW

make clean PLATFORM=MinGW

As the Makefiles were primarily created for OpenGL platforms (Linux and Mac OS), the MinGW build does not support Direct3D configurations. 


***********************************************************************
*** Running Demo Applications ***

* The demos may be run from the command line, or may be run directly by double-clicking on their executable files.  The demo executables are built into the same directory as the demo's source files.  The release executable is Example.exe, and the debug executable is ExampleD.exe.

* Some of the demos have no data files and may thus be run from any location.  However, numerous demos use local data files in the current directory.  As a result, it is best to run the demos from the directory to which they were built.


***********************************************************************
