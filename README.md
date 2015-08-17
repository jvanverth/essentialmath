Essential Math Sample Code
==========================

![alt text](https://images.tandf.co.uk/common/jackets/amazon/978148225/9781482250923.jpg "Book cover")

This depot contains the sample code and example applications referenced in the book [Essential Mathematics for Games and Interactive Applications](http://www.essentialmath.com/book.htm), along with additional supplementary code and files that should be of interest to readers of the book.

The following platforms are supported:

* Linux
* MacOS X (using Xcode)
* Windows (using Microsoft's Visual Studio development environment)

In addition to the demo code, we have included some text that was cut from the main text for length. These consist of two appendices reviewing trigonomety and calculus concepts, and a section of the first chapter covering fixed point numbers and arithmetic in more detail.

Installing The Libraries and Examples
-------------------------------------

You can clone the depot using

    git clone https://github.com/jvanverth/essentialmath.git
  
Or you can [download it directly](https://github.com/jvanverth/essentialmath/archive/master.zip).

Very little of the depot's contents are platform-specific, so all platforms are included in the same hierarchy.

Please consult the platform-specific README files for any additional platform-specific details regarding:
* System requirements
* Additional installation or configuration step
* How to build and run the demo applications

Supported platforms and their README files are as follows:
* Linux:   src/README_LINUX.md
* MacOS X: src/README_MACOS.md
* Windows:   src/README_WINDOWS.md

Source Directory Structure
--------------------------

The following is a listing of the directories and demos in src/:

/common - the set of libraries used in the book and by the demos

* /IvCollision - bounding volume classes and intersection methods
* /IvCurves - position-interpolation curve classes
* /IvEngine - classes and functions that support applications
* /IvEngine/OGL - OpenGL-specific application code
* /IvEngine/D3D11 - Direct3D 11 application code
* /IvGraphics - cross-platform rendering classes
* /IvGraphics/OGL - OpenGL-specific rendering code
* IvGraphics/D3D11 - Direct3D 11 rendering code
* /IvMath - basic 2-, 3-, and 4-dimensional vectors, 3x3 and 4x4 matrices, quaternions, rays, and other classes representing mathematical objects.
* /IvRandom - code for six simple random number generators
* /IvScene - classes implementing a basic hierarchical data structure
* /IvUtility - low-level system support code such as file I/O

&nbsp;&nbsp; In addition, after building, the /common directory will include two other directories, whose contents are derived from the libraries above:

* Includes: When built, will contain all of the .h files for all of the Iv libraries.  Building an Iv library (on any platform) will automatically copy that library's latest headers to this directory.  The demo applications all reference this directory (using relative paths) for all Iv* headers.  For those platforms using Makefiles, you will need to build the common directory to generate these files.

* Libs: When built, will contain Debug and Release subdirectories for each supported platform.  Building any Iv library on any platform will automatically update that library's target to the correct directory.  For those platforms using Makefiles, you will need to build the common directory to generate these files.

/Examples - all of the demo applications referenced by the book text

* /Ch04-Xforms - Contains the demos for Chapter 3: Affine Transformations

  * /Transforms-01-Interaction
  * /Transforms-02-Centered
  * /Transforms-03-Separate
  * /Transforms-04-Tank
  * /Transforms-05-Hierarchy

* /Ch05-Orientation - Contains the demos for Chapter 5: Orientation Representations

  * /Orientation-01-Transform

* /Ch06-Viewing - Contains the demos for Chapter 6: Viewing and Projection

  * /Viewing-01-LookAt
  * /Viewing-02-Rotation
  * /Viewing-03-Perspective
  * /Viewing-04-Stereo
  * /Viewing-05-Orthographic
  * /Viewing-06-Oblique
  * /Viewing-07-Clipping
  * /Viewing-08-Picking

* /Ch07-Interpolation - Contains the demos for Chapter 7: Interpolation

  * /Curves-01-Linear
  * /Curves-02-Hermite
  * /Curves-03-AutoHermite
  * /Curves-04-Catmull
  * /Curves-05-Lagrange
  * /Curves-06-Bezier
  * /Curves-07-B-Spline
  * /Curves-08-Euler
  * /Curves-09-LerpSlerp
  * /Curves-10-SpeedControl
  * /Curves-11-CameraControl

* /Ch08-GeometryColoring - Contains the demos for Chapter 8: Geometry and Textuting

  * /Geometry-01-ColorRemapping
  * /Geometry-02-BasicDrawing
  * /Geometry-03-BasicShading
  * /Geometry-04-UniformColors
  * /Geometry-05-VertexColors
  * /Geometry-06-SharpEdges
  * /Geometry-07-BasicTexturing
  * /Geometry-08-TextureAddressing

* /Ch09-Lighting - Contains the demos for Chapter 9: Lighting

  * /Lighting-01-DirectionalLight
  * /Lighting-02-PointLight
  * /Lighting-03-Spotlight
  * /Lighting-04-LightingComponents
  * /Lighting-05-MultipleLights
  * /Lighting-06-PerFragmentLighting
  * /Lighting-07-TexturesAndLighting
  * /Lighting-08-NormalMapping

* /Ch10-Raster - Contains the demos for Chapter 10: Rasterization

  * /Raster-01-DepthBuffering
  * /Raster-02-TextureFilter
  * /Raster-03-Mipmapping
  * /Raster-04-Blending

* /Ch11-RandomNumbers - Contains the demos for Chapter 11: Random Numbers

  *  /Random-01-SphereDisc
  * /Random-02-Perlin

* /Ch12-Collision - Contains the demos for Chapter 12: Intersection Testing

  * /Collision-01-Hierarchy
  * /Collision-02-SweepPrune

* /Ch13-Simulation -  Contains the demos for Chapter 13: Rigid Body Dynamics

  * /Simulation-01-Force
  * /Simulation-02-Torque
  * /Simulation-03-Tank
  * /Simulation-04-LinCollision
  * /Simulation-05-RotCollision
  * /Simulation-06-Friction

/glfw - Contains the GLFW library and headers, which handles window creation and event handling for the OpenGL demos. The build systems for the demo applications and Iv libraries on the necessary platforms reference this directory via relative paths when looking for GLFW headers and libraries.

/glew
* Contains the OpenGL GLEW library and headers for Windows and those Linux platforms that need it. This is used to handle OpenGL 3.0 support and extensions. The build systems for the demo applications and Iv libraries on the necessary platforms may reference this directory via relative paths when looking for GLEW headers and libraries.

/Windows
* Contains d3dcompiler_47.dll, which may be needed to build HLSL shaders when run as standalone apps.

License
-------
Copyright (c) 2015, James M. Van Verth and Lars M. Bishop.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of Essential Mathematics for Games and Interactive
Applications nor the names of its contributors may be used to endorse 
or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL JAMES M. VAN VERTH OR LARS M. BISHOP BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
