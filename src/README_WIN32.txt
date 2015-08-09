***********************************************************************
*** Win32 (Microsoft Visual Studio) Platform-Specific README FILE ***

If you have not already done so, please read the all-platform introductory file, /README_FIRST.TXT, which is located in the same directory as this file.


***********************************************************************
*** System Requirements / Installation ***

* This code distribution supports Windows 2000, Windows XP and Windows Vista

* The contents of the book's CDROM must be copied (keeping the hierarchy) to the local machine, as described in README_FIRST.TXT.

* Microsoft Microsoft Visual Studio 2005 or 2008 must be installed on the development machine.

* If you wish to use the OGL configurations, the file Win32System/glut32.DLL must be copied from this book's CDROM contents into:
	* Windows XP: /Windows/System32
	* Windows 2000: /Winnt/System32

* If you wish to use the D3D9 configurations, the latest DirectX 9.0 SDK should be installed on the development machine, and correct paths to the install directory should be set in the Visual Studio environment.


***********************************************************************
*** Microsoft Visual Studio Specifics ***

* The code on this disc was tested with Visual Studio 2005, Standard Edition and Visual Studio 2008, Express Edition.

* There are four configurations supported: a Debug and Release configuration for DirectX 9 and OpenGL, respectively.  

* The project and solution files were created by Visual Studio 2005, but can be converted to Visual Studio 2008.  Project files should convert automatically by double-clicking on them; solution files may need to be opened from within Visual Studio.


***********************************************************************
*** Building Libraries and Demo Applications ***

* For the OpenGL configurations, libraries and applications use the OpenGL GLUT and GLEW systems.  GLUT has been included on the CDROM in the top-level directory /gl.  All of the libraries and applications reference this directory via relative paths.  As a result, there should be no need to add any path to GLUT into your development tool settings.  However, see the note above regarding the GLUT runtime DLL.

The GLEW libraries can be found in the top-level directory /glew.  The libraries should work with both Visual Studio 2005 and Visual Studio 2008, but if they need to be rebuilt, a project file can be found under /glew/build/vc8.

* To build or rebuild all of the Iv* libraries in a single operation, open the "common" workspace:

	/common/Common.sln

and build all of the projects contained in it (IvCollision, IvCurves, IvEngine, IvGraphics, IvMath, IvRandom, IvScene and IvUtility).  The Batch Build option in Visual C++ is useful for this.  The projects may be compiled in any order.  For both platforms (OGL and D3D9), both release and debugging versions of the builds are supported.

* To build a single Iv* library (such as IvMath), either select and build the project from within the Common workspace described above, or open the .vcproj file for that library (e.g. /common/IvMath/IvMath.vcproj) and build the project. 

* The library .vcproj files (and, by inclusion, the Common.sln workspace as well) copy that library's header files to /common/Includes at the end of compilation.  In addition, the library itself is built to the /common/Libs subdirectory that matches the build configuration (Win32Release, Win32Debug, D3D9Release, D3D9Debug, OGLRelease or OGLDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

* To build any of the demo applications open the .sln for that particular application from the matching chapter and demo subdirectory of /Examples.  For example, the first demo for Chapter 4 may be found in:

	/Examples/Ch04-Xforms/Transforms-01-Interaction/Example.sln

* Each demo application .sln file includes the project files for any Iv libraries upon which the demo depends.  This allows for easy rebuilding of the necessary libraries, as well as making it easier to step through engine code as desired when working with the demos.


***********************************************************************
*** Running Demo Applications ***

* The demos may be run from the Microsoft development environment, or may be run directly by double-clicking on their executable files.  The demo executables are built into the same directory as the demo's .sln/.vcproj files.  The release executable for DirectX 9 is ExampleD3D9.exe, and the debug executable is ExampleD3D9_d.exe.  The release executable for OpenGL is ExampleOGL.exe, and the debug executable is ExampleOGL_d.exe.

* Some of the demos have no data files and may thus be run from any location.  However, numerous demos use local data files in the current directory.  As a result, it is best to run the demos from the directory to which they were built.


***********************************************************************
