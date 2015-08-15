Mac OS X Platform-Specific Instructions
=======================================

If you have not already done so, please read the all-platform introductory file, ../README_FIRST.md, which is located in the top directory.

System Requirements / Installation
----------------------------------

* This code distribution supports Mac OS 10.8 (Mountain Lion) through 10.10 (Yosemite).  It may work on earlier version of Mac OS X, but some project settings will need to be changed.

* The Xcode developer toolset must be installed.  These can be downloaded from https://developer.apple.com/.

Building Libraries and Demo Applications
----------------------------------------

The libraries and applications use the GLFW library for creating windows and capturing input events. Included is a pre-built library for Mac OS 10.8 and up. Should you need to rebuild this library, the source code is also included, and build instructions can be found at http://www.glfw.org/. Use the default build options. In particular, changing GLFW_USE_CHDIR may prevent some demos from running.

There is no need for GLEW, as the OpenGL 3.2 interfaces are already supported with the standard Mac installation.

To build or rebuild all of the Iv* libraries in a single operation, open the "common" project:

    src/common/Common.xcodeproj

and build all of the projects contained in it (IvCollision, IvCurves, IvEngine, IvGraphics, IvMath, IvRandom, IvScene and IvUtility).  The projects may be compiled in any order.  Both release and debugging versions of the builds are supported.

To build a single Iv* library (such as IvMath), either select and build the project from within the Common workspace described above, or open the .xcodeproj file for that library (e.g. /common/IvMath/IvMath.xcodeproj) and build the project. 

Unlike the other platforms, the library .xcodeproj files (and, by inclusion, the Common.xcodeproj file as well) will not copy that library's header files to /common/Includes at the end of compilation.  Instead, the include paths are explicitly set in each example's project file.  However, the library itself is built to the /common/Libs subdirectory that matches the build configuration (OSXRelease or OSXDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

To build any of the demo applications open the .xcodeproj for that particular application from the matching chapter and demo subdirectory of /Examples.  For example, the first demo for Chapter 4 may be found in:

    src/Examples/Ch04-Xforms/Transforms-01-Interaction/Example.xcodeproj

Each demo application .xcodeproj file includes the project files for any Iv libraries upon which the demo depends. This allows for easy rebuilding of the necessary libraries, as well as making it easier to step through engine code as desired when working with the demos.

Running Demo Applications
-------------------------

The demos can either be run from within the Finder or from within Xcode itself.  The demo executables are built into the same directory as the demo's source files.  The release executable is Example.app, and the debug executable is ExampleD.app.

Some of the demos have data files. These files are bundled into the application, so they may be run from any location.




