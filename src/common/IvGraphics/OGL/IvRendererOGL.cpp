//===============================================================================
// @ IvRendererOGL.cpp
// 
// Windowing and display setup routines
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include "IvDebugger.h"
#include "IvAssert.h"
#include "IvIndexBufferOGL.h"
#include <IvMath.h>
#include "IvRendererOGL.h"
#include "IvResourceManagerOGL.h"
#include "IvShaderProgramOGL.h"
#include "IvVertexBufferOGL.h"
#include "IvUniform.h"
#include "IvMatrix33.h"

#ifdef PLATFORM_OSX
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif


//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static GLint sPrimTypeMap[kPrimTypeCount];

static IvShaderProgramOGL* sDefaultShaders[kVertexFormatCount];

static GLint sBlendSrcFunc[kBlendFuncCount];
static GLint sBlendDestFunc[kBlendFuncCount];

static GLint sDepthFunc[kDepthTestCount];

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvRenderer::Create()
//-------------------------------------------------------------------------------
// Static constructor
//-------------------------------------------------------------------------------
bool 
IvRendererOGL::Create()
{
	if ( !mRenderer )
		mRenderer = new IvRendererOGL();
    return ( mRenderer != 0 );

}   // End of IvRendererOGL::Create()


//-------------------------------------------------------------------------------
// @ IvRendererOGL::IvRendererOGL()
//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
IvRendererOGL::IvRendererOGL() : IvRenderer()
{
    mShader = NULL;

	mAPI = kOpenGL;

//	GLfloat diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

    sPrimTypeMap[kPointListPrim] = GL_POINTS;
    sPrimTypeMap[kLineListPrim] = GL_LINES;
    sPrimTypeMap[kLineStripPrim] = GL_LINE_STRIP;
    sPrimTypeMap[kTriangleListPrim] = GL_TRIANGLES;
    sPrimTypeMap[kTriangleStripPrim] = GL_TRIANGLE_STRIP;

    sBlendSrcFunc[kNoBlendFunc] = GL_ONE;
    sBlendDestFunc[kNoBlendFunc] = GL_ZERO;
    sBlendSrcFunc[kOpacityBlendFunc] = GL_SRC_ALPHA;
    sBlendDestFunc[kOpacityBlendFunc] = GL_ONE_MINUS_SRC_ALPHA;
    sBlendSrcFunc[kAddBlendFunc] = GL_ONE;
    sBlendDestFunc[kAddBlendFunc] = GL_ONE;
    sBlendSrcFunc[kMultiplyBlendFunc] = GL_ZERO;
    sBlendDestFunc[kMultiplyBlendFunc] = GL_SRC_COLOR;

    sDepthFunc[kDisableDepthTest] = GL_ALWAYS;
    sDepthFunc[kGreaterDepthTest] = GL_GREATER;
    sDepthFunc[kGreaterEqualDepthTest] = GL_GEQUAL;
    sDepthFunc[kLessDepthTest] = GL_LESS;
    sDepthFunc[kLessEqualDepthTest] = GL_LEQUAL;

}   // End of IvRendererOGL::IvRendererOGL()


//-------------------------------------------------------------------------------
// @ IvRendererOGL::~IvRendererOGL()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
IvRendererOGL::~IvRendererOGL()
{
	// delete shader program?
    for ( unsigned int i = 0; i < kVertexFormatCount; ++i )
    {
        if (sDefaultShaders[i])
        {
            mResourceManager->Destroy(sDefaultShaders[i]);
            sDefaultShaders[i] = 0;
        }
    }

    if (mResourceManager)
    {
        delete (IvResourceManagerOGL*)(mResourceManager);
        mResourceManager = 0;
    }

}   // End of IvRendererOGL::~IvRendererOGL()
    

//-------------------------------------------------------------------------------
// @ IvRendererOGL::Initialize()
//-------------------------------------------------------------------------------
// Initialize display
//-------------------------------------------------------------------------------
bool
IvRendererOGL::Initialize( unsigned int width, unsigned int height )
{
    if (!InitGL())
        return false;

    // set GL parameters for window size
    Resize( width, height );
    
    // create resource manager
    mResourceManager = new IvResourceManagerOGL;

    return true;
    
}   // End of IvRendererOGL::Initialize()
    
    
//-------------------------------------------------------------------------------
// @ IvRendererOGL::Resize()
//-------------------------------------------------------------------------------
// Set up GL parameters for window size
//-------------------------------------------------------------------------------
void 
IvRendererOGL::Resize(unsigned int width, unsigned int height ) 
{
    // prevent divide by zero
    if (height == 0)                                    
    {
        height = 1;                                 
    }

    // set up current viewport
    glViewport(0,0,width,height);                       
    mWidth = width;
    mHeight = height;

    // set default projection matrix
    float d = 1.0f/IvTan(mFOV/180.0f*kPI*0.5f);
    float recip = 1.0f/(mNear-mFar);
    IvMatrix44 perspective;

    perspective(0,0) = d/((GLfloat)width/(GLfloat)height);
    perspective(1,1) = d;
    perspective(2,2) = (mNear+mFar)*recip;
    perspective(2,3) = 2*mNear*mFar*recip;
    perspective(3,2) = -1.0f;
    perspective(3,3) = 0.0f;

    SetProjectionMatrix(perspective);

    IvMatrix44 ident;

    SetViewMatrix(ident);
    SetWorldMatrix(ident);

}   // End of IvRendererOGL::Resize()


//-------------------------------------------------------------------------------
// @ IvRendererOGL::InitGL()
//-------------------------------------------------------------------------------
// Set up base GL parameters
//-------------------------------------------------------------------------------
int 
IvRendererOGL::InitGL()
{
    // turn on smooth shading
//****    glShadeModel(GL_SMOOTH);
    
    // set clear color and depth
    SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    SetClearDepth(1.0f); 
    
    // set up depth buffer
    glEnable(GL_DEPTH_TEST);                            
    glDepthFunc(GL_LEQUAL); 
    
    // set up perspective correct textures
//****    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // use sRGB gamma
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    // turn on culling
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glPointSize( 5.0f );

    return true;                                        

}   // End of IvRendererOGL::InitGL()


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetClearColor()
//-------------------------------------------------------------------------------
// Set background color
//-------------------------------------------------------------------------------
void 
IvRendererOGL::SetClearColor( float red, float green, float blue, float alpha )  
{
    // set clear color
    glClearColor( red, green, blue, alpha );   
}
 

//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetClearDepth()
//-------------------------------------------------------------------------------
// Set background depth
//-------------------------------------------------------------------------------
void 
IvRendererOGL::SetClearDepth( float depth )  
{
    // set clear depth
    glClearDepth( depth );   
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::ClearBuffers()
//-------------------------------------------------------------------------------
// Clear necessary buffers
//-------------------------------------------------------------------------------
void IvRendererOGL::ClearBuffers(IvClearBuffer buffer)
{
    switch (buffer)
    {
        case kColorClear:
            glClear( GL_COLOR_BUFFER_BIT );
            break;
        case kDepthClear:
            glClear( GL_DEPTH_BUFFER_BIT );
            break;
        case kColorDepthClear:
        default:
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            break;
    };
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetBlendFunc()
//-------------------------------------------------------------------------------
// Set the pixel-blending function
//-------------------------------------------------------------------------------
void IvRendererOGL::SetBlendFunc(IvBlendFunc blend)
{
    if (blend == kNoBlendFunc)
        glDisable(GL_BLEND);
    else
        glEnable(GL_BLEND);

    glBlendFunc(sBlendSrcFunc[blend], sBlendDestFunc[blend]);
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetColorMask()
//-------------------------------------------------------------------------------
// Set which colors will actually be written to the color buffer
//-------------------------------------------------------------------------------
void IvRendererOGL::SetColorMask( bool red, bool green, bool blue, bool alpha )
{
    glColorMask(red, green, blue, alpha);
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetDepthTest()
//-------------------------------------------------------------------------------
// Set the depth-testing function
//-------------------------------------------------------------------------------
void IvRendererOGL::SetDepthTest(IvDepthTestFunc func)
{
    if (func == kDisableDepthTest)
        glDisable(GL_DEPTH_TEST);
    else
        glEnable(GL_DEPTH_TEST);

    glDepthFunc(sDepthFunc[func]);
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::GetDepthTest()
//-------------------------------------------------------------------------------
// Get the depth-testing function
//-------------------------------------------------------------------------------
IvDepthTestFunc IvRendererOGL::GetDepthTest()
{
	GLint mode;
	glGetIntegerv( GL_DEPTH_TEST, &mode );
    if (mode == GL_FALSE)
	{
        return kDisableDepthTest;
	}

	glGetIntegerv( GL_DEPTH_FUNC, &mode );
    for (unsigned int func = 0; func < kDepthTestCount; ++func)
    {
        if ( mode == sDepthFunc[func] )
            return (IvDepthTestFunc)(func);
    }

    ASSERT(false);
    return kLessEqualDepthTest;
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetFillMode()
//-------------------------------------------------------------------------------
// Set whether we're in solid or wireframe drawing mode
//-------------------------------------------------------------------------------
void IvRendererOGL::SetFillMode( IvFillMode fill )
{
    if (fill == kWireframeFill)
    {
#ifndef PLATFORM_OSX
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable( GL_CULL_FACE );
#else
        glPolygonMode(GL_FRONT, GL_LINE);
#endif        
    }
    else if (fill == kSolidFill)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable( GL_CULL_FACE );
    }
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
void IvRendererOGL::SetShadeMode( IvShadeMode shade )
{
//    if (shade == kFlatShaded)
//        glShadeModel(GL_FLAT);
//    else if (shade == kGouraudShaded)
//        glShadeModel(GL_SMOOTH);
//
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::GetShadeMode()
//-------------------------------------------------------------------------------
// Set whether we're in flat or Gouraud shading mode
//-------------------------------------------------------------------------------
IvShadeMode 
IvRendererOGL::GetShadeMode()
{
//	GLint mode;
//	glGetIntegerv( GL_SHADE_MODEL, &mode );
//    if (mode == GL_FLAT)
//	{
//        return kFlatShaded;
//	}
//    else if (mode == GL_SMOOTH)
//	{
//		return kGouraudShaded;
//	}
//	else
//	{
//		ASSERT(false);
//		return kGouraudShaded;
//	}
    return kGouraudShaded;
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetDepthWrite()
//-------------------------------------------------------------------------------
// Enable/Disable the writing of depths to the depth buffer
//-------------------------------------------------------------------------------
void IvRendererOGL::SetDepthWrite(bool write)
{
    glDepthMask(write ? GL_TRUE : GL_FALSE);
}

//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetWorldMatrix()
//-------------------------------------------------------------------------------
// Sets the world matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererOGL::SetWorldMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetWorldMatrix(matrix);
}

//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetViewMatrix()
//-------------------------------------------------------------------------------
// Sets the camera matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererOGL::SetViewMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetViewMatrix(matrix);
}

//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetProjectionMatrix()
//-------------------------------------------------------------------------------
// Sets the projection matrix for the renderer
//-------------------------------------------------------------------------------
void IvRendererOGL::SetProjectionMatrix(const IvMatrix44& matrix)
{
    IvRenderer::SetProjectionMatrix(matrix);
}

//-------------------------------------------------------------------------------
// @ IvRendererOGL::GetShaderProgram()
//-------------------------------------------------------------------------------
// Returns the currently-active shader program
//-------------------------------------------------------------------------------
IvShaderProgram* IvRendererOGL::GetShaderProgram()
{
    return mShader;
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::SetShaderProgram()
//-------------------------------------------------------------------------------
// Applies a shader program
//-------------------------------------------------------------------------------
void IvRendererOGL::SetShaderProgram(IvShaderProgram* program)
{
    // This is a safe cast, since we will only link IvShaderProgramOGL with the
    // OGL renderer.
    mShader = static_cast<IvShaderProgramOGL*>(program);
    if (mShader)
    {
        mShader->MakeActive();
    }
    else
    {
        // Otherwise (NULL shader), we will need to bind a default shader later 
        glUseProgram( 0 );
    }
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererOGL::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, 
                  IvIndexBuffer* indexBuffer, unsigned int numIndices)
{
    BindDefaultShaderIfNeeded(vertexBuffer->GetVertexFormat());

	// update any default uniforms
	if ( mShader )
	{
		IvUniform* modelviewproj = mShader->GetUniform("IvModelViewProjectionMatrix");
		if ( modelviewproj )
		{
			modelviewproj->SetValue(mWVPMat, 0);
		}
		IvUniform* normalMat = mShader->GetUniform("IvNormalMatrix");
		if ( normalMat )
		{
			normalMat->SetValue(mNormalMat, 0);
		}
		IvUniform* diffuseColor = mShader->GetUniform("IvDiffuseColor");
		if (diffuseColor)
		{
			diffuseColor->SetValue(mDiffuseColor,0);
		}
		IvUniform* ambient = mShader->GetUniform("IvLightAmbient");
		if ( ambient )
		{
			ambient->SetValue(mLightAmbient,0);
		}
		IvUniform* diffuse = mShader->GetUniform("IvLightDiffuse");
		if ( diffuse )
		{
			diffuse->SetValue(mLightDiffuse,0);
		}
		IvUniform* direction = mShader->GetUniform("IvLightDirection");
		if ( direction )
		{
			direction->SetValue(mLightDirection,0);
		}
	}

    if (vertexBuffer)
        static_cast<IvVertexBufferOGL*>(vertexBuffer)->MakeActive();
    else
        return;

    if (indexBuffer)
        static_cast<IvIndexBufferOGL*>(indexBuffer)->MakeActive();
    else
        return;

    glDrawElements(sPrimTypeMap[primType], numIndices, GL_UNSIGNED_INT, 0);
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::Draw()
//-------------------------------------------------------------------------------
// Draws the given buffers
//-------------------------------------------------------------------------------
void IvRendererOGL::Draw(IvPrimType primType, IvVertexBuffer* vertexBuffer, unsigned int numVertices)
{
    BindDefaultShaderIfNeeded(vertexBuffer->GetVertexFormat());

	// update any default uniforms
	if ( mShader )
	{
		IvUniform* modelviewproj = mShader->GetUniform("IvModelViewProjectionMatrix");
		if ( modelviewproj )
		{
			modelviewproj->SetValue(mWVPMat, 0);
		}
		IvUniform* normalMat = mShader->GetUniform("IvNormalMatrix");
		if ( normalMat )
		{
			normalMat->SetValue(mNormalMat, 0);
		}
		IvUniform* diffuseColor = mShader->GetUniform("IvDiffuseColor");
		if (diffuseColor)
		{
			diffuseColor->SetValue(mDiffuseColor,0);
		}
		IvUniform* ambient = mShader->GetUniform("IvLightAmbient");
		if ( ambient )
		{
			ambient->SetValue(mLightAmbient,0);
		}
		IvUniform* diffuse = mShader->GetUniform("IvLightDiffuse");
		if ( diffuse )
		{
			diffuse->SetValue(mLightDiffuse,0);
		}
		IvUniform* direction = mShader->GetUniform("IvLightDirection");
		if ( direction )
		{
			direction->SetValue(mLightDirection,0);
		}
	}

    if (vertexBuffer)
        static_cast<IvVertexBufferOGL*>(vertexBuffer)->MakeActive();
    else
        return;

    glDrawArrays(sPrimTypeMap[primType], 0, numVertices);
}


//-------------------------------------------------------------------------------
// @ IvRendererOGL::BindDefaultShaderIfNeeded()
//-------------------------------------------------------------------------------
// Selects a default shader for the given vertex format if none is bound
//-------------------------------------------------------------------------------
void IvRendererOGL::BindDefaultShaderIfNeeded(IvVertexFormat format)
{
    if (mShader)
        return;

    if (!sDefaultShaders[format])
    {
        IvVertexShader* vs = mResourceManager->CreateDefaultVertexShader(format); 
        IvFragmentShader* fs = mResourceManager->CreateDefaultFragmentShader(format);

        sDefaultShaders[format] = static_cast<IvShaderProgramOGL*>(
            mResourceManager->CreateShaderProgram( vs, fs ));
    }

    SetShaderProgram(sDefaultShaders[format]);
}


