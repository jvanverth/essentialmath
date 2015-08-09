IVTARGET = IvGraphicsOGL
ifeq ($(PLATFORM), OSX)
IVINCLUDEDIRS = -I../../IvUtility -I../../IvMath -I../ -I../../..
else
IVINCLUDEDIRS = -I../../IvUtility -I../../IvMath -I../ -I../../.. -I../../../glew/include
endif
COMMONDIR = ../..
TARGETSUFFIX = /OGL
include ../../MakefileCommon
