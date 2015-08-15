Windows Platform-Specific Instructions
======================================

If you have not already done so, please read the all-platform introductory file, ../README_FIRST.md, which is located in the top directory.

System Requirements / Installation
----------------------------------

This code distribution has been tested under Windows 7 and Windows 8. It's expected that it will work under Windows 10, but has not been tested.

Microsoft Microsoft Visual Studio 2013 or later must be installed on the development machine. Visual Studio Community is sufficient.

If you wish to use the D3D11 configurations and run the executables outside of Visual Studio, you may need to add src/Windows into your path via the PATH environment variable. This includes the Win32 shared library d3dcompiler_47.dll which is used to compile the HLSL shader files. Alternatively, you can copy d3dcompiler_47.dll into the same directory as any demo executable, or copy it to C:\Windows.

If you get an error saying something like "Cannot open D3DCompiler_4x.dll", the DLL we have given you is not the right one for your system. Try looking in your Direct3D setup, for example C:\Program Files (x86)\Windows Kits\8.1\bin\x86, for the latest version. You will need the Win32 DLL, not the Win64 DLL, as the projects are compiled to 32-bit.

Microsoft Visual Studio Specifics
---------------------------------

* The code on this disc was tested with Visual Studio Express and Visual Studio Community 2013.

* There are four configurations supported: a Debug and Release configuration for DirectX 11 and OpenGL, respectively.  

* The project and solution files were created by Visual Studio 2013, but should be able to converted to any later version.  Project files should convert automatically by double-clicking on them; solution files may need to be opened from within Visual Studio.

Building Libraries and Demo Applications
----------------------------------------

For the OpenGL configurations, libraries and applications use the OpenGL GLFW and GLEW systems.  The source and prebuilt libraries for GLFW have been included.  All of the libraries and applications reference the GLFW directory via relative paths.  As a result, there should be no need to add any path to GLFW into your development tool settings. The GLEW libraries can be found in the top-level directory /glew.  Again prebuilt libraries are included and referenced via relative paths.

Source code for both GLFW and GLEW should not be needed, but is included in case it needs to be rebuilt. Instructions for building GLFW can be found at http://www.glfw.org/, and those for GLEW can be found at http://glew.sourceforge.net/.

To build or rebuild all of the Iv* libraries in a single operation, open the "common" workspace:

    /common/Common.sln

and build all of the projects contained in it (IvCollision, IvCurves, IvEngine, IvGraphics, IvMath, IvRandom, IvScene and IvUtility).  For both platforms (OGL and D3D11), both release and debugging versions of the builds are supported.

To build a single Iv* library (such as IvMath), either select and build the project from within the Common workspace described above, or open the .vcproj file for that library (e.g. /common/IvMath/IvMath.vcproj) and build the project. 

The library .vcproj files (and, by inclusion, the Common.sln workspace as well) copy that library's header files to /common/Includes at the end of compilation.  In addition, the library itself is built to the /common/Libs subdirectory that matches the build configuration (Win32Release, Win32Debug, D3D9Release, D3D9Debug, OGLRelease or OGLDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

To build any of the demo applications open the .sln for that particular application from the matching chapter and demo subdirectory of /Examples.  For example, the first demo for Chapter 4 may be found in:

    src/Examples/Ch04-Xforms/Transforms-01-Interaction/Example.sln

Each demo application .sln file includes the project files for any Iv libraries upon which the demo depends.  This allows for easy rebuilding of the necessary libraries, as well as making it easier to step through engine code as desired when working with the demos.

Running Demo Applications
-------------------------

The demos may be run from the Microsoft development environment, or may be run directly by double-clicking on their executable files.  The demo executables are built into the same directory as the demo's .sln/.vcproj files.  The release executable for DirectX 11 is ExampleD3D11.exe, and the debug executable is ExampleD3D11_d.exe.  The release executable for OpenGL is ExampleOGL.exe, and the debug executable is ExampleOGL_d.exe.

Some of the demos have no data files and may thus be run from any location.  However, numerous demos use local data files in the current directory.  As a result, it is best to run the demos from the directory to which they were built.

When running the demos outside of Visual Studio, you may see errors regarding d3dcompiler_47.dll. In that case you will either need to copy d3dcompiler_47.dll from the src/Windows directory to the directory containing the executable, or copy it to C:\Windows, or change your PATH environment variable to add the src/Windows directory to your default search path.
