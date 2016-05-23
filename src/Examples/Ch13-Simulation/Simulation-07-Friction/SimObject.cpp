//===============================================================================
// @ SimObject.cpp
// ------------------------------------------------------------------------------
//
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This is the base class for simulation objects.  It handles integration of 
// linear and angular factors, and collision detection and response.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvResourceManager.h>
#include <IvRendererHelp.h>
#include <IvMatrix44.h>
#include <IvMath.h>

#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>

#include "SimObject.h"

#include "Game.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

static IvVertexBuffer* gStoneVertices = 0;
static IvIndexBuffer* gStoneIndices = 0;

extern bool mUseFriction;

const float kFrictionCoeff = 0.43f;

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ SimObject::SimObject()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
SimObject::SimObject()
{
    mRotate.Identity();
    mTranslate.Zero();

    mMass = 17.0f;
    mElasticity = 0.7f;   // varies between 0 and 1
    mSphere.SetRadius( 1.0f );

    mVelocity.Zero();
    mConstantForce.Zero();
    mAngularMomentum.Zero();
    mAngularVelocity.Zero();
    mConstantTorque.Zero();

    InitializeMoments();

}   // End of SimObject::SimObject()


//-------------------------------------------------------------------------------
// @ SimObject::~SimObject()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
SimObject::~SimObject()
{
}   // End of SimObject::~SimObject()


//-------------------------------------------------------------------------------
// @ SimObject::Reset()
//-------------------------------------------------------------------------------
// Reset sim values
//-------------------------------------------------------------------------------
void
SimObject::Reset()
{
    mRotate.Identity();
    mTranslate.Zero();
    mVelocity.Zero();
    mConstantForce.Zero();
    mAngularMomentum.Zero();
    mAngularVelocity.Zero();
    mConstantTorque.Zero();
    
}   // End of SimObject::Reset()

//-------------------------------------------------------------------------------
// @ SimObject::SetMass()
//-------------------------------------------------------------------------------
// Set mass and reinitialize moments of inertia
//-------------------------------------------------------------------------------
void
SimObject::SetMass( float mass )
{
    mMass = mass;

    InitializeMoments();

}   // End of SimObject::SetBounds()


//-------------------------------------------------------------------------------
// @ SimObject::SetBounds()
//-------------------------------------------------------------------------------
// Set bounding data and reinitialize moments of inertia
//-------------------------------------------------------------------------------
void
SimObject::SetBounds( float radius )
{
    mSphere.SetRadius( radius );
    
    InitializeMoments();

}   // End of SimObject::SetBounds()


//-------------------------------------------------------------------------------
// @ SimObject::InitializeMoments()
//-------------------------------------------------------------------------------
// Initialize moments of inertia
//-------------------------------------------------------------------------------
void
SimObject::InitializeMoments()
{
    // set moments of inertia for disk-like torus
    float radiussq = mSphere.GetRadius();
    radiussq *= radiussq;
    mMomentsInverse.Identity();
    mMomentsInverse(0,0) = 2.0f*mMass*radiussq/5.0f;
    mMomentsInverse(1,1) = 2.0f*mMass*radiussq/5.0f;
    mMomentsInverse(2,2) = 2.0f*mMass*radiussq/5.0f;
    mMomentsInverse.Inverse();

    mWorldMomentsInverse = mMomentsInverse;
    
}   // End of SimObject::InitializeMoments()


//-------------------------------------------------------------------------------
// @ SimObject::SetTranslate()
//-------------------------------------------------------------------------------
// Set translation for object
//-------------------------------------------------------------------------------
void SimObject::SetTranslate( const IvVector3& position )
{
    mTranslate = position;
    mSphere.SetCenter( position );

}   // End of SimObject::SetTranslate()


//-------------------------------------------------------------------------------
// @ SimObject::Update()
//-------------------------------------------------------------------------------
// Update object
//-------------------------------------------------------------------------------
void
SimObject::Update( float dt )
{
    Integrate( dt );

    mSphere.SetCenter( mTranslate );

}   // End of SimObject::Update()


//-------------------------------------------------------------------------------
// @ SimObject::Integrate()
//-------------------------------------------------------------------------------
// Move to next time step
//-------------------------------------------------------------------------------
void 
SimObject::Integrate( float dt )                                    
{   
    // compute acceleration
    IvVector3 accel = 1.0f/mMass * CurrentForce( mTranslate, mVelocity, 
                                                 mRotate, mAngularVelocity );
    // clear small values
    accel.Clean();          

    // compute new velocity, position
    mVelocity += dt*accel;
    // clear small values
    mVelocity.Clean();      
    mTranslate += dt*mVelocity;
    
    IvVector3 torque = CurrentTorque( mTranslate, mVelocity, mRotate, mAngularVelocity );

    // compute new angular momentum, orientation
    mAngularMomentum += dt*torque;
    mAngularMomentum.Clean();
    
    // update angular velocity
    IvMatrix33 rotateMat(mRotate);
    mWorldMomentsInverse = rotateMat*mMomentsInverse*Transpose(rotateMat);
    mAngularVelocity = mWorldMomentsInverse*mAngularMomentum;
    mAngularVelocity.Clean();

    IvQuat w(mAngularVelocity);
    mRotate += 0.5f*dt*(w*mRotate);
    mRotate.Normalize();
    mRotate.Clean();

}   // End of SimObject::Integrate()


//-------------------------------------------------------------------------------
// @ SimObject::CurrentForce()
//-------------------------------------------------------------------------------
// Compute torque, given forces constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
SimObject::CurrentForce( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalForce = mConstantForce;
    // add some drag
    totalForce -= 0.25f*mMass*velocity;
    
    totalForce += 0.01f*mMass*(angularVelocity.Cross(velocity));

    return totalForce;

}   // End of SimObject::CurrentForce()


//-------------------------------------------------------------------------------
// @ SimObject::CurrentTorque()
//-------------------------------------------------------------------------------
// Compute torque, given torques constant over interval and variable params 
//-------------------------------------------------------------------------------
IvVector3 
SimObject::CurrentTorque( const IvVector3& position, const IvVector3& velocity,
                       const IvQuat& orientation, const IvVector3& angularVelocity )                                
{   
    IvVector3 totalTorque = mConstantTorque;
    // fake some angular friction -- should use moments of inertia, really
    totalTorque -= 0.25f*mMass*angularVelocity; 

    return totalTorque;

}   // End of SimObject::CurrentTorque()


//-------------------------------------------------------------------------------
// @ SimObject::HandleCollision()
//-------------------------------------------------------------------------------
// Handle collisions between this object and another one
//-------------------------------------------------------------------------------
void
SimObject::HandleCollision( SimObject* other )
{
    // do sphere check
    IvVector3 collisionNormal, collisionPoint;
    float penetration;

    // if two objects are colliding
    if ( Colliding(other, collisionNormal, collisionPoint, penetration) )
    {
        // handle collision

        // push out by penetration, scaled by relative masses
        float relativeMass = mMass/(mMass + other->mMass);
        mTranslate -= (1.0f-relativeMass)*penetration*collisionNormal;
        other->mTranslate += relativeMass*penetration*collisionNormal;
        
        // compute relative velocity
        IvVector3 r1 = collisionPoint - mTranslate;
        IvVector3 r2 = collisionPoint - other->mTranslate;
        IvVector3 vel1 = mVelocity + Cross( mAngularVelocity, r1 );
        IvVector3 vel2 = other->mVelocity + Cross( other->mAngularVelocity, r2 );
        IvVector3 relativeVelocity = vel1 - vel2;
        
        // determine if we're heading away from each other
        float vDotN = relativeVelocity.Dot( collisionNormal );
        if (vDotN < 0)
            return;
        
        // compute impulse factor
        float denominator = (1.0f/mMass + 1.0f/other->mMass);
        
        // compute angular factors
        IvVector3 cross1 = Cross(r1, collisionNormal);
        IvVector3 cross2 = Cross(r2, collisionNormal);
        cross1 = mWorldMomentsInverse*cross1;
        cross2 = other->mWorldMomentsInverse*cross2;
        IvVector3 sum = Cross(cross1, r1) + Cross(cross2, r2);
        denominator += (sum.Dot(collisionNormal));
        
        // Note: this is different from the presentation in the book,
        // which is incorrect. There is a single coefficient of restitution
        // for each pair of objects. The following is a reasonable approximation,
        // assuming that mElasticity represents the coefficient of resitution
        // between this object and a perfectly elastic surface.
        float restitution = mElasticity*other->mElasticity;
        // compute j
        float modifiedVel = vDotN/denominator;
        float j_n = -(1.0f+restitution)*modifiedVel;
        IvVector3 impulse = j_n*collisionNormal;
        IvVector3 otherImpulse = -impulse;
        
        // manage friction (assuming dynamic friction)
        if (mUseFriction)
        {
            IvVector3 collisionTangent = relativeVelocity - relativeVelocity.Dot(collisionNormal)*collisionNormal;
            collisionTangent.Normalize();
            
            // compute for us first
            float f_k = kFrictionCoeff*IvAbs(j_n)/mMass;
            impulse -= f_k*collisionTangent;
            
            // now for them
            f_k = kFrictionCoeff*IvAbs(j_n)/other->mMass;
            otherImpulse -= f_k*collisionTangent;
        }
        
        // update velocities
        mVelocity += impulse/mMass;
        other->mVelocity += otherImpulse/other->mMass;
        
        // update angular velocities
        mAngularMomentum += Cross(r1, impulse);
        mAngularVelocity = mWorldMomentsInverse*mAngularMomentum;
        other->mAngularMomentum -= Cross(r2, impulse);
        other->mAngularVelocity = other->mWorldMomentsInverse*other->mAngularMomentum;
    }

}   // End of SimObject::HandleCollision()


//-------------------------------------------------------------------------------
// @ SimObject::Colliding()
//-------------------------------------------------------------------------------
// Check to see if colliding with another SimObject
//-------------------------------------------------------------------------------
bool 
SimObject::Colliding( const SimObject* other, IvVector3& collisionNormal, 
                      IvVector3& collisionPoint, float& penetration ) const
{
    return mSphere.ComputeCollision( other->mSphere, collisionNormal, 
                              collisionPoint, penetration );

}   // End of SimObject::Colliding()


//-------------------------------------------------------------------------------
// @ SimObject::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
SimObject::Render()                                 
{   
    // build 4x4 matrix
    IvMatrix44 transform( mRotate );
    transform(0,3) = mTranslate.x;
    transform(1,3) = mTranslate.y;
    transform(2,3) = mTranslate.z;
    
    IvSetWorldMatrix( transform );
    
    // clear to default shader
    IvShaderProgram* oldShader = IvRenderer::mRenderer->GetShaderProgram();
    IvRenderer::mRenderer->SetShaderProgram(0);
    
    // create matrix to scale to radius
    transform.Scaling(IvVector3(1.0f,1.0f,1.0f));
    
    // concatenate with the current matrix
    IvMatrix44 currentMat = IvRenderer::mRenderer->GetWorldMatrix();
    
    // multiply by transform
    IvRenderer::mRenderer->SetWorldMatrix(currentMat * transform);
    
    // draw it
    IvColor color = kCyan;
    IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    IvRenderer::mRenderer->Draw(kTriangleListPrim, gStoneVertices, gStoneIndices);
    
    IvMatrix44 rotate;
    rotate.Rotation(kPI, 0.0f, 0.0f);
    
    IvRenderer::mRenderer->SetWorldMatrix(currentMat * rotate * transform);
    color = kRed;
    IvRenderer::mRenderer->SetDefaultDiffuseColor(color.mRed/255.f, color.mGreen/255.f, color.mBlue/255.f, color.mAlpha/255.f);
    IvRenderer::mRenderer->Draw(kTriangleListPrim, gStoneVertices, gStoneIndices);
    
    // restore original state
    IvRenderer::mRenderer->SetDefaultDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    IvRenderer::mRenderer->SetShaderProgram( oldShader );
    IvRenderer::mRenderer->SetWorldMatrix(currentMat);
}

//-------------------------------------------------------------------------------
// @ SimObject::CreateStoneGeometry()
//-------------------------------------------------------------------------------
// Set up global vertex and index buffer
//-------------------------------------------------------------------------------
void SimObject::CreateStoneGeometry()
{
    // load data if not there
    if (gStoneVertices == 0)
    {
        const int stacks = 16;
        const int slices = 12;

        size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();

        int numVerts = slices*(stacks - 1) + 2;
        IvNPVertex* vertexPtr = (IvNPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kNPFormat] * numVerts);
        int numIndices = 6 * slices*(stacks - 1);
        UInt32* indexPtr = (UInt32*)IvStackAllocator::mScratchAllocator->Allocate(sizeof(UInt32) * numIndices);

        int currentVertex = 0;
        int currentIndex = 0;

        // verts
        vertexPtr[currentVertex].normal.Set(0.0, 0.0f, -1.0f);
        vertexPtr[currentVertex].position.Set(0.0, 0.0f, -1.0f);
        currentVertex++;
        const float increment = kPI / (float)stacks;
        const float thetaIncrement = kTwoPI / (float)(2 * slices - 2);

        for (UInt32 latitude = 1; latitude < stacks; ++latitude)
        {
            for (UInt32 longitude = 0; longitude < slices; ++longitude)
            {
                float phi = -kHalfPI + float(latitude)*increment;
                float theta = float(longitude)*thetaIncrement;

                float sinTheta, cosTheta;
                IvSinCos(theta, sinTheta, cosTheta);

                float sinPhi, cosPhi;
                IvSinCos(phi, sinPhi, cosPhi);

                vertexPtr[currentVertex].normal.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);
                vertexPtr[currentVertex].position.Set(cosTheta*cosPhi, sinTheta*cosPhi, sinPhi);

                if (longitude > 0)
                {
                    if (currentVertex >= 1 + slices)
                    {
                        indexPtr[currentIndex++] = currentVertex + 0;
                        indexPtr[currentIndex++] = currentVertex - 1;
                        indexPtr[currentIndex++] = currentVertex - slices - 1;
                        indexPtr[currentIndex++] = currentVertex + 0;
                        indexPtr[currentIndex++] = currentVertex - slices - 1;
                        indexPtr[currentIndex++] = currentVertex - slices;
                    }
                    else
                    {
                        indexPtr[currentIndex++] = 0;
                        indexPtr[currentIndex++] = currentVertex;
                        indexPtr[currentIndex++] = currentVertex - 1;
                    }
                }
                currentVertex++;
            }
            if (currentVertex >= 2 * slices)
            {
                indexPtr[currentIndex++] = currentVertex - slices;
                indexPtr[currentIndex++] = currentVertex - 1;
                indexPtr[currentIndex++] = currentVertex - slices - 1;
                indexPtr[currentIndex++] = currentVertex - slices;
                indexPtr[currentIndex++] = currentVertex - slices - 1;
                indexPtr[currentIndex++] = currentVertex - 2 * slices;
            }
            else
            {
                indexPtr[currentIndex++] = 0;
                indexPtr[currentIndex++] = currentVertex - slices;
                indexPtr[currentIndex++] = currentVertex - 1;
            }
        }

        vertexPtr[currentVertex].normal.Set(0.0, 0.0f, 1.0f);
        vertexPtr[currentVertex].position.Set(0.0, 0.0f, 1.0f);

        for (UInt32 longitude = 0; longitude < slices - 1; ++longitude)
        {
            indexPtr[currentIndex++] = currentVertex;
            indexPtr[currentIndex++] = currentVertex - longitude - 2;
            indexPtr[currentIndex++] = currentVertex - longitude - 1;
        }
        indexPtr[currentIndex++] = currentVertex;
        indexPtr[currentIndex++] = currentVertex - 1;
        indexPtr[currentIndex++] = currentVertex - slices;

        gStoneVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kNPFormat, numVerts, vertexPtr, kImmutableUsage);
        gStoneIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(numIndices, indexPtr, kImmutableUsage);

        if (!gStoneVertices || !gStoneIndices)
        {
            if (gStoneVertices)
            {
                IvRenderer::mRenderer->GetResourceManager()->Destroy(gStoneVertices);
                gStoneVertices = 0;
            }
            if (gStoneIndices)
            {
                IvRenderer::mRenderer->GetResourceManager()->Destroy(gStoneIndices);
                gStoneIndices = 0;
            }
        }

        IvStackAllocator::mScratchAllocator->Reset(currentOffset);
    }
}

//-------------------------------------------------------------------------------
// @ SimObject::DestroyStoneGeometry()
//-------------------------------------------------------------------------------
// Delete up global vertex and index buffer
//-------------------------------------------------------------------------------
void SimObject::DestroyStoneGeometry()
{
    if (gStoneVertices)
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(gStoneVertices);
        gStoneVertices = 0;
    }
    if (gStoneIndices)
    {
        IvRenderer::mRenderer->GetResourceManager()->Destroy(gStoneIndices);
        gStoneIndices = 0;
    }
}