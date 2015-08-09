***********************************************************************
*** Introduction ***

Welcome to the CDROM companion to the book:

Essential Mathematics for Games and Interactive Applications 
	- A Programmer's Guide

This CDROM contains the sample code and example applications referenced in the text, along with additional supplementary code that should be of interest to readers of the book.

Please read the /LICENSE.TXT file at the top level of the CDROM.  It contains detailed legal information regarding legal uses of the code on this CDROM.

The code on this CDROM supports the following platforms:

* Linux

* MacOS X

* MinGW (MinGW is an open-source development environment for Win32 systems)

* Windows (using Microsoft's Visual Studio development environment)


***********************************************************************
*** Installing The Libraries and Examples ***

On all platforms, the first installation step is to copy the entire contents of the CDROM to a directory on the target machine, making sure to keep the hierarchy intact.  Very little of the CDROM contents are platform-specific, so all platforms are included in the same hierarchy.  If you wish to do development make sure that the files are writable.  

Please consult the platform-specific README files for any additional platform-specific details regarding:
* System requirements
* Additional installation or configuration step
* How to build and run the demo applications

Supported platforms and their README files are as follows:

* Linux:   /README_LINUX.TXT
* MacOS X: /README_MACOS.TXT
* MinGW:   /README_MINGW.TXT 
* Win32:   /README_WIN32.TXT

***********************************************************************
*** Top-level Directories ***

The CDROM contains four top-level directories:

/common
* Contains the Iv* math, rendering, animation and collision libraries referenced in the text and used by the demo applications.

/Examples
* Contains all of the demo applications referenced in the text.

/gl
* Contains the OpenGL GLUT headers and libraries for all platforms that do not ship with GLUT support.  The build systems for the demo applications and Iv libraries on the necessary platforms reference this directory via relative paths when looking for GLUT headers and libraries.

/glew
* Contains the OpenGL GLEW headers and libraries for all platforms that do not ship with built-in OpenGL 2.0 and GLSL support.  The build systems for the demo applications and Iv libraries on the necessary platforms reference this directory via relative paths when looking for GLEW headers and libraries.

/Win32System
* Contains DLL(s) (such as GLUT32.DLL) that are required by the libraries and demo applications on the Win32 platforms.  See the platform-specific README files for details on whether or not your platform requires these files to be installed elsewhere on the system.


***********************************************************************
*** The Iv* Libraries ***

The /common directory includes subdirectories for each of the Iv libraries.  These libraries are used by some or all of the demo applications.  The libraries (which bear the same name as the directory containing them) include:

* IvCollision: Implements basic bounding volume creation, management and intersection.

* IvCurves: Implements positional interpolation methods.

* IvEngine: Implements basic application-level framework classes.

* IvGraphics: Implements cross-platform rendering classes.

* IvMath: Implements basic 2-, 3-, and 4-dimensional vectors, 3x3 and 4x4 matrices, Quaternions, rays, and other classes representing mathematical objects.

* IvRandom: Implements four simple random number generators.

* IvScene: Implements a very simple hierarchical scene graph.

* IvUtility: Implements numerous low-level support classes.

In addition, the /common directory includes two other directories, whose contents are derived from the libraries above:

* Includes: When built, will contain all of the .h files for all of the Iv libraries.  Building an Iv library (on any platform) will automatically copy that library's latest headers to this directory.  The demo applications all reference this directory (using relative paths) for all Iv* headers.  For those platforms using Makefiles, you will need to build the common directory to generate these files.

* Libs: When built, will contain Debug and Release subdirectories for each supported platform.  Building any Iv library on any platform will automatically update that library's target to the correct directory.  For those platforms using Makefiles, you will need to build the common directory to generate these files.


***********************************************************************
*** Files and Demos on the CDROM ***

The following is a listing of the directories and demos on the CDROM:

/common
* Contains the set of libraries used in the book and by the demos

    /Includes
    * Contains copies of all of the headers from the Iv* directories

    /IvCollision
    * Contains bounding volume classes and intersection methods

    /IvCurves
    * Contains position-interpolation curve classes

    /IvEngine
    * Contains classes and functions that support applications
    
    /IvEngine/OGL
    * Contains OpenGL-specific application code
 
    /IvEngine/D3D9
    * Contains Direct3D9-specific application code

    /IvGraphics
    * Implements four simple random number generators.

    /IvGraphics/OGL
    * Contains OpenGL-specific rendering code
 
    /IvGraphics/D3D9
    * Contains Direct3D9-specific rendering code

    /IvMath
    * Contains foundation mathematical classes such as vectors

    /IvRandom
    * Contains code for four simple random number generators.

    /IvScene
    * Contains classes implementing a basic hierarchical scene graph

    /IvUtility
    * Contains low-level system support code such as file I/O

/Examples
* Contains all of the demo applications referenced by the book text

    /Ch04-Xforms
    * Contains the demos for Chapter 3: Affine Transformations

        /Transforms-01-Interaction

        /Transforms-02-Centered

        /Transforms-03-Separate

        /Transforms-04-Tank

        /Transforms-05-SceneGraph

    /Ch05-Orientation
    * Contains the demos for Chapter 5: Orientation Representations

        /Orientation-01-Transform

    /Ch06-Viewing
    * Contains the demos for Chapter 6: Viewing and Projection

        /Viewing-01-LookAt

        /Viewing-02-Rotation

        /Viewing-03-Perspective

        /Viewing-04-Stereo

        /Viewing-05-Orthographic

        /Viewing-06-Oblique

        /Viewing-07-Clipping

        /Viewing-08-Picking

    /Ch07-GeometryColoring
    * Contains the demos for Chapter 7: Geometry and Textuting

        /Geometry-01-ColorRemapping

        /Geometry-02-BasicDrawing

        /Geometry-03-BasicShading

        /Geometry-04-UniformColors

        /Geometry-05-VertexColors

        /Geometry-06-SharpEdges

        /Geometry-07-BasicTexturing

        /Geometry-08-TextureAddressing

    /Ch08-Lighting
    * Contains the demos for Chapter 8: Lighting

        /Lighting-01-DirectionalLight

        /Lighting-02-PointLight

        /Lighting-03-Spotlight

        /Lighting-04-LightingComponents

        /Lighting-05-MultipleLights

        /Lighting-06-PerFragmentLighting

        /Lighting-07-TexturesAndLighting

        /Lighting-08-NormalMapping

    /Ch09-Raster
    * Contains the demos for Chapter 9: Rasterization

        /Raster-01-DepthBuffering

        /Raster-02-TextureFilter

        /Raster-03-Mipmapping

        /Raster-04-Blending

    /Ch10-Interpolation
    * Contains the demos for Chapter 10: Interpolation

        /Curves-01-Linear

        /Curves-02-Hermite

        /Curves-03-AutoHermite

        /Curves-04-Catmull

        /Curves-05-Lagrange

        /Curves-06-Bezier

        /Curves-07-B-Spline

        /Curves-08-Euler

        /Curves-09-LerpSlerp

        /Curves-10-SpeedControl

        /Curves-11-CameraControl

    /Ch11-RandomNumbers
    * Contains the demos for Chapter 11: Random Numbers

        /Random-01-SphereDisc

        /Random-02-Perlin

    /Ch12-Collision
    * Contains the demos for Chapter 12: Intersection Testing

        /Collision-01-Hierarchy

        /Collision-02-SweepPrune

    /Ch13-Simulation
    * Contains the demos for Chapter 13: Rigid Body Dynamics

        /Simulation-01-Force

        /Simulation-02-Torque

        /Simulation-03-Tank

        /Simulation-04-LinCollision

        /Simulation-05-AngCollision

/gl
* Contains the GLUT headers and libraries needed to build the code on Win32 systems

/glew
* Contains the GLEW headers and libraries needed to build the code on Win32 systems

/Win32System
* Contains Windows runtime DLL(s) for the GLUT system


***********************************************************************
*** Updates: ***

The latest updates and/or fixes to the source code for this book are available on the Internet (to original owners of a copy of the book).  The URL is http://www.essentialmath.com.


***********************************************************************
*** Known Issues: ***

* On all platforms that build from Makefiles (i.e. Linux, MacOS and MinGW), minimal builds are not supported.  Any build will be a full rebuild of that target.  In other words, building in the /common directory will rebuild all libraries from scratch.  Building in the /common/IvMath directory will rebuild all IvMath source files, and so on.

* IvMatrix33 and IvMatrix44 do not include methods that create reflections or shears.

* Fast versions of sqrt(x) and 1/sqrt(x) have been implemented, but not tested.

***********************************************************************
