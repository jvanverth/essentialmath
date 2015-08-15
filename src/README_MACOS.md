***********************************************************************
*** Mac OS X Platform-Specific README FILE ***

If you have not already done so, please read the all-platform introductory file, /README_FIRST.TXT, which is located in the same directory as this file.


***********************************************************************
*** System Requirements / Installation ***

* This code distribution supports Mac OS X 10.4 (Tiger) and 10.5 (Leopard).  It may work on Mac OS X 10.2 and 10.3, but it hasn't been tested.

* The contents of the book's CDROM must be copied (keeping the hierarchy) to the local machine, as described in README_FIRST.TXT

* The Mac OS X Developer Tools must be installed.  These can usually be found on a separate disk, as part of your OS suite.

* Before building examples you will need to create the necessary libraries.
See "Building Libraries and Demo Applications", for more details.

* Examples can be built using either Makefiles or Xcode, though the method used   will affect how the example can be run.  See below under "Running Demo Applications".


***********************************************************************
*** MacOS Specifics ***

The system must have the Mac OS X Developer Tools installed. 

This CDROM was built and tested using Mac OS 10.5.2 and Xcode 3.0.

It is best not to mix Xcode-built and Makefile-built libraries.  Use either one method or the other.

***********************************************************************
*** Building Libraries and Demo Applications ***

The libraries and applications use the OpenGL GLUT system.  GLUT should be included as part of the Developer Tools installation.  As a result, there should be no need to add any path to GLUT into your development tool settings.  There is also no need for GLEW, as the OpenGL 2.0 interfaces are
already supported with the standard Mac installation.

***********************************************************************
*** Building Using Makefiles ***

The MacOS platform can use "make" from a command prompt to build any or all of the libraries and demo applications.  A hierarchical build system allows the user to rebuild at any desired level in the code hierarchy.

To build, open a Terminal shell and change directory to the desired directory:

* To build or rebuild all Iv libraries in a single command, change directories to /common and build as described below.

* To build or rebuild a specific Iv library (such as IvMath), change directories to the /common subdirectory for the library (e.g. /common/IvMath) and build as described below.

* The library makefiles copy that library's header files to /common/Includes at the end of compilation.  In addition, the library itself is built to the /common/Libs subdirectory that matches the build configuration (OSXRelease or OSXDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

* To rebuild all demo applications for all chapters in a single command, change directories to /Examples and build as described below.

* To rebuild all demo applications for a specific chapter in a single command (such as Chapter 4), change directories to that chapter's subdirectory under /Examples (e.g. /Examples/Ch04-Xforms) and build as described below.

* To rebuild a specific demo application, change directories to that demo's subdirectory under /Examples/Ch* and build as described below.

* To build, note that MacOS IS NOT THE DEFAULT PLATFORM.  To build any part of the code for MacOS, use the following commands in the desired build directory:

* For release builds:

make PLATFORM=OSX

OR

make release PLATFORM=OSX

* For debugging builds:

make debug PLATFORM=OSX

* To clear out all builds of the current directory for MacOS

make clean PLATFORM=OSX


***********************************************************************
*** Building Using Xcode ***

* To build or rebuild all of the Iv* libraries in a single operation, open the "common" project:

	/common/Common.xcodeproj

and build all of the projects contained in it (IvCollision, IvCurves, IvEngine, IvGraphics, IvMath, IvRandom, IvScene and IvUtility).  The projects may be compiled in any order.  Both release and debugging versions of the builds are supported.

* To build a single Iv* library (such as IvMath), either select and build the project from within the Common workspace described above, or open the .xcodeproj file for that library (e.g. /common/IvMath/IvMath.xcodeproj) and build the project. 

* Unlike the other platforms, the library .xcodeproj files (and, by inclusion, the Common.xcodeproj file as well) will not copy that library's header files to /common/Includes at the end of compilation.  Instead, the include paths are explicitly set in each example's project file.  However, the library itself is built to the /common/Libs subdirectory that matches the build configuration (OSXRelease or OSXDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

* To build any of the demo applications open the .xcodeproj for that particular application from the matching chapter and demo subdirectory of /Examples.  For example, the first demo for Chapter 4 may be found in:

	/Examples/Ch04-Xforms/Transforms-01-Interaction/Example.xcodeproj

* Each demo application .xcodeproj file includes the project files for any Iv libraries upon which the demo depends.  This allows for easy rebuilding of the necessary libraries, as well as making it easier to step through engine code as desired when working with the demos.


***********************************************************************
*** Running Demo Applications ***

* The demos are best run from the command line (if using Makefiles) or from within Xcode (if using Xcode).  The demo executables are built into the same directory as the demo's source files.  When using Makefiles, the release executable is Example, and the debug executable is ExampleD.  When using Xcode, the release executable is Example.app, and the debug executable is ExampleD.app.

* Some of the demos have no data files and may thus be run from any location.  However, numerous demos use local data files in the current directory. As a result, it is best to run the demos from the directory to which they were built.

* For demos without local files, double-clicking will work with both build options.  However, because of how the Mac handles the current directory, double-clicking on the executables will not work if there are local data files.  In addition, any executables built using Xcode that require local files will not work even from the command line -- they must be run from within Xcode.  

***********************************************************************




