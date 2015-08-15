Linux Platform-Specific Instructions
====================================

If you have not already done so, please read the all-platform introductory file, ../README.md, which is located in the main directory.

System Requirements / Installation
----------------------------------

* This code distribution supports Linux.  It was built and tested using Ubuntu Linux 15.04 (Vivid Vervet).

* The contents of the distribution must be copied (keeping the hierarchy) to the local machine, as described in README_FIRST.TXT

* Packages for OpenGL and GLEW must be installed, and the GLFW library must be built - see the section "OpenGL Specifics" below for details

* Before building examples you will need to create the necessary libraries.
See "Building Libraries and Demo Applications", for more details.

OpenGL Specifics
----------------

The system must have development packages for OpenGL and GLEW installed.  These should be part of a standard Linux distribution, although they may not be installed by default.  For example, under Ubuntu, you may need to use the package manager to install them:

> sudo apt-get install mesa-common-dev libglew-dev

The path to OpenGL and GLEW include and library files may be different on your system.  If so, you will need to change the SYSIPATH variable in /common/MakefileCommon, and the SYSLPATH variable in /Examples/MakefileExamples. If GLEW is not available on your system, the necessary build files can be found in src/glew-1.12.0/.

In addition, you will need to build the GLFW library. Again, the necessary dependencies may not be installed, namely Cmake and the xorg libraries. The following will build GLFW under Ubuntu:

> sudo apt-get install cmake xorg-dev
> cd src/glfw-3.1.1
> cmake .
> make

However it is built, the Makefiles are expecting to find libglfw3.a in src/glfw-3.1.1/src/.

Finally, you should be sure that you are using the correct drivers for your video card.  

Building Libraries and Demo Applications
----------------------------------------

The libraries and applications use the OpenGL GLFW and GLEW systems.  As mentioned, for best results you will need to make sure these are installed on your system.

The Linux platform uses "make" from a command prompt to build any or all of the libraries and demo applications.  A hierarchical build system allows the user to rebuild at any desired level in the code hierarchy.

To build, open a command prompt and change directory to the desired directory:

* To build or rebuild all Iv libraries in a single command, change directories to /common and build as described below.

* To build or rebuild a specific Iv library (such as IvMath), change directories to the /common subdirectory for the library (e.g. /common/IvMath) and build as described below.

* The library makefiles copy that library's header files to /common/Includes at the end of compilation.  In addition, the library itself is built to the /common/Libs subdirectory that matches the build configuration (LinuxRelease or LinuxDebug).  However, the libraries themselves reference other Iv library headers directly from that library's source directory.  As a result, the libraries may be built in any order.

* To rebuild all demo applications for all chapters in a single command, change directories to /Examples and build as described below.

* To rebuild all demo applications for a specific chapter in a single command (such as Chapter 4), change directories to that chapter's subdirectory under /Examples (e.g. /Examples/Ch04-Xforms) and build as described below.

* To rebuild a specific demo application, change directories to that demo's subdirectory under /Examples/Ch* and build as described below.

* To build any part of the code for Linux, use the following commands in the desired build directory:

* For release builds:

> make

OR

> make release

* For debugging builds:

> make debug

* To clear out all builds of the current directory for Linux

> make clean

Running Demo Applications
-------------------------

* The demos may be run from the command line, or may be run directly.  The demo executables are built into the same directory as the demo's source files.  The release executable is Example.elf, and the debug executable is ExampleD.elf.

* Some of the demos have no data files and may thus be run from any location.  However, numerous demos use local data files in the current directory.  As a result, it is best to run the demos from the directory to which they were built.



