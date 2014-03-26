//===============================================================================
// @ IvCovariance.cpp
// 
// Helper routines for computing properties of covariance matrices
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvAssert.h>
#include "IvCovariance.h"
#include <IvMatrix33.h>
#include <IvVector3.h>
#include <IvMath.h>
#include <string.h>

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IvComputeCovarianceMatrix()
//-------------------------------------------------------------------------------
// Computes and returns the real, symmetric covariance matrix, along with the
// mean vector.
//-------------------------------------------------------------------------------
void IvComputeCovarianceMatrix( IvMatrix33& C, IvVector3& mean,
                              const IvVector3* points, unsigned int numPoints )
{
    ASSERT(numPoints > 1);

    mean = IvVector3::origin;

    // compute the mean (the centroid of the points)
    unsigned int i;
    for (i = 0; i < numPoints; i++)
        mean += points[i];

    mean /= (float)numPoints;

    // compute the (co)variances
    float varX = 0.0f;
    float varY = 0.0f;
    float varZ = 0.0f;
    float covXY = 0.0f;
    float covXZ = 0.0f;
    float covYZ = 0.0f;

    for (i = 0; i < numPoints; i++)
    {
        IvVector3 diff = points[i] - mean;
        
        varX += diff.GetX() * diff.GetX();
        varY += diff.GetY() * diff.GetY();
        varZ += diff.GetZ() * diff.GetZ();
        covXY += diff.GetX() * diff.GetY();
        covXZ += diff.GetX() * diff.GetZ();
        covYZ += diff.GetY() * diff.GetZ();
    }

    // divide all of the (co)variances by n - 1 
    // (skipping the division if n = 2, since that would be division by 1.0
    if (numPoints > 2)
    {
        const float normalize = (float)(numPoints - 1);
        varX /= normalize;
        varY /= normalize;
        varZ /= normalize;
        covXY /= normalize;
        covXZ /= normalize;
        covYZ /= normalize;
    }

    // pack values into the covariance matrix, which is symmetric
    C(0,0) = varX;
    C(1,1) = varY;
    C(2,2) = varZ;
    C(1,0) = C(0,1) = covXY;
    C(2,0) = C(0,2) = covXZ;
    C(1,2) = C(2,1) = covYZ;

}  // End of IvComputeCovarianceMatrix()


//-------------------------------------------------------------------------------
// @ IvSymmetricHouseholder3x3()
//-------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------
static void IvSymmetricHouseholder3x3 (const IvMatrix33& M, IvMatrix33& Q, 
                                    IvVector3& diag, IvVector3& subd)
{
    // Computes the Householder reduction of M, computing:
    //
    // T = Q^t M Q
    //
    //   Input:   
    //     symmetric 3x3 matrix M
    //   Output:  
    //     orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, lower-triangle entries of T (T is symmetric)

    // T will be stored as follows (because it is symmetric and tridiagonal):
    //
    // | diag[0]  subd[0]  0       |
    // | subd[0]  diag[1]  subd[1] |
    // | 0        subd[1]  diag[2] |

    diag[0] = M(0,0); // in both cases below, T(0,0) = M(0,0) 
    subd[2] = 0; // T is to be a tri-diagonal matrix - the (2,0) and (0,2) 
                 // entries must be zero, so we don't need subd[2] for this step

    // so we know that T will actually be:
    //
    // | M(0,0)   subd[0]  0       |
    // | subd[0]  diag[1]  subd[1] |
    // | 0        subd[1]  diag[2] |

    // so the only question remaining is the lower-right block and subd[0]

    if ( IvAbs(M(2,0)) < kEpsilon )
    {
        // if M(2,0) (and thus M(0,2)) is zero, then the matrix is already in
        // tridiagonal form.  As such, the Q matix is the identity, and we
        // just extract the diagonal and subdiagonal components of T as-is
        Q.Identity();

        // so we see that T will actually be:
        //
        // | M(0,0)  M(1,0)  0      |
        // | M(1,0)  M(1,1)  M(2,1) |
        // | 0       M(2,1)  M(2,2) |
        diag[1] = M(1,1);
        diag[2] = M(2,2);

        subd[0] = M(1,0);
        subd[1] = M(2,1);
    }
    else
    {
        // grab the lower triangle of the matrix, minus a, which we don't need
        // (see above)
        // |       |
        // | b d   |
        // | c e f |
        const float b = M(1,0);
        const float c = M(2,0);
        const float d = M(1,1);
        const float e = M(2,1);
        const float f = M(2,2);

        // normalize b and c to unit length and store in u and v
        // we want the lower-right block we create to be orthonormal
        const float L = IvSqrt(b * b + c * c);
        const float u = b / L;
        const float v = c / L;
        Q.SetRows(IvVector3(1.0f, 0.0f, 0.0f),
                  IvVector3(0.0f, u,    v),
                  IvVector3(0.0f, v,    -u));

        float q = 2 * u * e + v * (f - d);
        diag[1] = d + v * q;
        diag[2] = f - v * q;

        subd[0] = L;
        subd[1] = e - u * q;

        // so we see that T will actually be:
        //
        // | M(0,0)  L       0     |
        // | L       d+c*q   e-b*q |
        // | 0       e-b*q   f-c*q |
    }
}  // End of IvSymmetricHouseholder3x3

//-------------------------------------------------------------------------------
// @ IvQLAlgorithm()
//-------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------
static int IvQLAlgorithm (IvMatrix33& M, IvVector3& diag, IvVector3& subd)
{
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal

    int L;
    for (L = 0; L < 3; L++)
    {
        // As this is an iterative process, we need to keep a maximum number of
        // iterations, just in case something is wrong - we cannot afford to
        // loop forever
        const int maxIterations = 32;

        int iter;
        for (iter = 0; iter < maxIterations; iter++)
        {
            int m;
            for (m = L; m <= 1; m++)
            {
                float dd = IvAbs(diag[m]) + IvAbs(diag[m+1]);
                if ( IvAbs(subd[m]) + dd == dd )
                    break;
            }
            if ( m == L )
                break;

            float g = (diag[L+1]-diag[L])/(2*subd[L]);
            float r = IvSqrt(g*g+1);
            if ( g < 0 )
                g = diag[m]-diag[L]+subd[L]/(g-r);
            else
                g = diag[m]-diag[L]+subd[L]/(g+r);
            float s = 1, c = 1, p = 0;
            for (int i = m-1; i >= L; i--)
            {
                float f = s*subd[i], b = c*subd[i];
                if ( IvAbs(f) >= IvAbs(g) )
                {
                    c = g/f;
                    r = IvSqrt(c*c+1);
                    subd[i+1] = f*r;
                    c *= (s = 1/r);
                }
                else
                {
                    s = f/g;
                    r = IvSqrt(s*s+1);
                    subd[i+1] = g*r;
                    s *= (c = 1/r);
                }
                g = diag[i+1]-p;
                r = (diag[i]-g)*s+2*b*c;
                p = s*r;
                diag[i+1] = g+p;
                g = c*r-b;

                for (int k = 0; k < 3; k++)
                {
                    f = M(k,i+1);
                    M(k,i+1) = s*M(k,i)+c*f;
                    M(k,i) = c*M(k,i)-s*f;
                }
            }
            diag[L] -= p;
            subd[L] = g;
            subd[m] = 0;
        }

        // exceptional case - matrix took more iterations than should be 
        // possible to move to diagonal form
        if ( iter == maxIterations )
            return 0;
    }

    return 1;
}  // End of IvQLAlgorithm

//-------------------------------------------------------------------------------
// @ IvGetRealSymmetricEigenvectors()
//-------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------
void IvGetRealSymmetricEigenvectors( IvVector3& v1, IvVector3& v2, IvVector3& v3, 
                                   const IvMatrix33& A )
{    
    IvVector3 eigenvalues;
    IvVector3 lowerTriangle;
    IvMatrix33 Q;

    IvSymmetricHouseholder3x3 (A, Q, eigenvalues, lowerTriangle);
    IvQLAlgorithm(Q, eigenvalues, lowerTriangle);

    // Sort the eigenvalues from greatest to smallest, and use these indices
    // to sort the eigenvectors
    int v1Index, v2Index, v3Index;
    if (eigenvalues[0] > eigenvalues[1])
    {
        if (eigenvalues[1] > eigenvalues[2])
        {
            v1Index = 0;
            v2Index = 1;
            v3Index = 2;
        }
        else if (eigenvalues[2] > eigenvalues[0])
        {
            v1Index = 2;
            v2Index = 0;
            v3Index = 1;
        }
        else
        {
            v1Index = 0;
            v2Index = 2;
            v3Index = 1;
        }
    }
    else // eigenvalues[1] >= eigenvalues[0]
    {
        if (eigenvalues[0] > eigenvalues[2])
        {
            v1Index = 1;
            v2Index = 0;
            v3Index = 2;
        }
        else if (eigenvalues[2] > eigenvalues[1])
        {
            v1Index = 2;
            v2Index = 1;
            v3Index = 0;
        }
        else
        {
            v1Index = 1;
            v2Index = 2;
            v3Index = 0;
        }
    }

    // Sort the eigenvectors into the output vectors
    v1 = Q.GetColumn(v1Index);
    v2 = Q.GetColumn(v2Index);
    v3 = Q.GetColumn(v3Index);

    // If the resulting eigenvectors are left-handed, negate the 
    // min-eigenvalue eigenvector to make it right-handed
    if ( v1.Dot(v2.Cross(v3)) < 0.0f )
        v3 = -v3;
}  // End of IvGetRealSymmetricEigenvectors

