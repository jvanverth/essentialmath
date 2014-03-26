IVTARGET = IvEngineOGL
ifeq ($(PLATFORM), OSX)
IVINCLUDEDIRS = -I../../IvGraphics -I../../IvUtility -I../../IvMath -I../ -I../../..
else
IVINCLUDEDIRS = -I../../IvGraphics -I../../IvUtility -I../../IvMath -I../ -I../../.. -I../../../glew/include
endif
COPYHEADERS = False
COMMONDIR = ../..
TARGETSUFFIX = /OGL
include ../../MakefileCommon
