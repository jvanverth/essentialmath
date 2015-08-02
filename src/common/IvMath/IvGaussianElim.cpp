//----------------------------------------------------------------------------
// @ IvGaussianElim.cpp
// 
// Function to perform solve linear system using Gaussian elimination
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
// Implementation notes:
//     Matrix must be stored in column major order
//===============================================================================

//----------------------------------------------------------------------------
//-- Includes ----------------------------------------------------------------
//----------------------------------------------------------------------------

#include <string.h>

#include "IvGaussianElim.h"
#include "IvMath.h"
#include "IvDebugger.h"

//----------------------------------------------------------------------------
//-- Functions ---------------------------------------------------------------
//----------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ ::Solve()
//-------------------------------------------------------------------------------
// Perform Gaussian elimination to solve linear system
// Will destroy original values of A and b
// Result is returned in b
//-------------------------------------------------------------------------------
bool 
Solve( float* b, float* A, unsigned int n )
{
    // do one pass for each diagonal element
    for ( unsigned int pivot = 0; pivot < n; ++pivot )
    {
        unsigned int row, col;  // counters

        // find the largest magnitude element in the current column
        unsigned int maxrow = pivot;
        float maxelem = ::IvAbs( A[ maxrow + n*pivot ] );
        for ( row = pivot+1; row < n; ++row )
        {
            float elem = ::IvAbs( A[ row + n*pivot ] );
            if ( elem > maxelem )
            {
                maxelem = elem;
                maxrow = row;
            }
        }

        // if max is zero, stop!
        if ( IvIsZero( maxelem ) )
        {
            ERROR_OUT( "::Solve() -- singular matrix\n" );
            return false;
        }

        // if not in the current row, swap rows
        if ( maxrow != pivot )
        {
            float temp;
            // swap the row
            for ( col = 0; col < n; ++col )
            {
                temp = A[ maxrow + n*col ];
                A[ maxrow + n*col ] = A[ pivot + n*col ];
                A[ pivot + n*col ] = temp;
            }
            // swap elements in solution vector
            temp = b[ maxrow ];
            b[ maxrow ] = b[ pivot ];
            b[ pivot ] = temp;
        }
       
        // multiply current row by 1/pivot to "set" pivot to 1
        float pivotRecip = 1.0f/A[ n*pivot + pivot ];
        for ( col = 0; col < n; ++col )
        {
            A[ pivot + n*col ] *= pivotRecip;
        }
        b[ pivot ] *= pivotRecip;

        // Set pivot to exactly 1, to be sure
        A[pivot + n*pivot] = 1.0f;

        // now zero out pivot column in other rows 
        for ( row = pivot+1; row < n; ++row )
        {
            // subtract multiple of pivot row from current row,
            // such that pivot column element becomes 0
            float factor = A[ row + n*pivot ];

            // subtract multiple of row
            for ( col = 0; col < n; ++col )
            {
                A[ row + n*col ] -= factor*A[ pivot + n*col ];   
            }
            b[ row ] -= factor*b[ pivot ];
        }
    }

    // done, do backwards substitution 
    unsigned int p = n-1;
    do
    {
        --p;
        // subtract multiples of other known entities
        for ( unsigned int col = p+1; col < n; ++col )
        {
            b[p] -= A[ p + n*col ]*b[col];
        }
    }
    while (p > 0); 

    return true;
}


//-------------------------------------------------------------------------------
// @ ::InvertMatrix()
//-------------------------------------------------------------------------------
// Invert matrix using Gaussian elimination
//-------------------------------------------------------------------------------
bool
InvertMatrix( float* A, unsigned int n )
{
    unsigned int* swap;       // which row have we swapped with the current one?
    swap = new unsigned int[n];

    // do one pass for each diagonal element
    for ( unsigned int pivot = 0; pivot < n; ++pivot )
    {
        unsigned int row, col;  // counters

        // find the largest magnitude element in the current column
        unsigned int maxrow = pivot;
        float maxelem = IvAbs( A[ maxrow + n*pivot ] );
        for ( row = pivot+1; row < n; ++row )
        {
            float elem = IvAbs( A[ row + n*pivot ] );
            if ( elem > maxelem )
            {
                maxelem = elem;
                maxrow = row;
            }
        }

        // if max is zero, stop!
        if ( IvIsZero( maxelem ) )
        {
            ERROR_OUT( "::Inverse() -- singular matrix\n" );
            delete [] swap;
            return false;
        }

        // if not in the current row, swap rows
        swap[pivot] = maxrow;
        if ( maxrow != pivot )
        {
            // swap the row
            for ( col = 0; col < n; ++col )
            {
                float temp = A[ maxrow + n*col ];
                A[ maxrow + n*col ] = A[ pivot + n*col ];
                A[ pivot + n*col ] = temp;
            }
        }
       
        // multiply current row by 1/pivot to "set" pivot to 1
        float pivotRecip = 1.0f/A[ n*pivot + pivot ];
        for ( col = 0; col < n; ++col )
        {
            A[ pivot + n*col ] *= pivotRecip;
        }

        // copy 1/pivot to pivot point (doing inverse in place)
        A[pivot + n*pivot] = pivotRecip;

        // now zero out pivot column in other rows 
        for ( row = 0; row < n; ++row )
        {
            // don't subtract from pivot row
            if ( row == pivot )
                continue;
               
            // subtract multiple of pivot row from current row,
            // such that pivot column element becomes 0
            float factor = A[ row + n*pivot ];

            // clear pivot column element (doing inverse in place)
            // will end up setting this element to -factor*pivotInverse
            A[ row + n*pivot ] = 0.0f;
            
            // subtract multiple of row
            for ( col = 0; col < n; ++col )
            {
                A[ row + n*col ] -= factor*A[ pivot + n*col ];   
            }
        }
    }

    // done, undo swaps in column direction, in reverse order
    unsigned int p = n;
    do
    {
        --p;
        // if row has been swapped
        if (swap[p] != p)
        {
            // swap the corresponding column
            for ( unsigned int row = 0; row < n; ++row )
            {
                float temp = A[ row + n*swap[p] ];
                A[ row + n*swap[p] ] = A[ row + n*p ];
                A[ row + n*p ] = temp;
            }
        }
    }
    while (p > 0);

    delete [] swap;

    return true;

}   // End of IvMatrix33::Inverse()


//-------------------------------------------------------------------------------
// @ ::Determinant()
//-------------------------------------------------------------------------------
// Get determinant of matrix
// Uses Gaussian elimination to create an upper triangular matrix,
// multiplies diagonal to get intermediate value, then adjusts by row operations
// Will not destroy A
//-------------------------------------------------------------------------------
float 
Determinant( float* A, unsigned int n )
{
    // copy the original matrix
    float* copy = new float[n*n];
    memcpy( copy, A, sizeof(float)*n*n );

    // initial value for determinant
    float det = 1.0f;

    // do one pass for each diagonal element
    for ( unsigned int pivot = 0; pivot < n; ++pivot )
    {
        unsigned int row, col;  // counters

        // find the largest magnitude element in the current column
        unsigned int maxrow = pivot;
        float maxelem = IvAbs( copy[ maxrow + n*pivot ] );
        for ( row = pivot+1; row < n; ++row )
        {
            float elem = IvAbs( copy[ row + n*pivot ] );
            if ( elem > maxelem )
            {
                maxelem = elem;
                maxrow = row;
            }
        }

        // if max is zero, stop!
        if ( IvIsZero( maxelem ) )
        {
            // singular matrix, determinant is 0
            delete [] copy;
            return 0.0f;
        }

        // if not in the current row, swap rows
        if ( maxrow != pivot )
        {
            float temp;
            // swap the row
            for ( col = 0; col < n; ++col )
            {
                temp = copy[ maxrow + n*col ];
                copy[ maxrow + n*col ] = copy[ pivot + n*col ];
                copy[ pivot + n*col ] = temp;
            }
            // negate determinant
            det = -det;
        }
       
        // multiply current row by 1/pivot to "set" pivot to 1
        float pivotRecip = 1.0f/copy[ pivot + n*pivot ];
        for ( col = 0; col < n; ++col )
        {
            copy[ pivot + n*col ] *= pivotRecip;
        }
        det *= pivotRecip;

        // Set pivot to exactly 1, to be sure
        copy[ pivot + n*pivot] = 1.0f;

        // now zero out pivot column in other rows 
        for ( row = pivot+1; row < n; ++row )
        {
            // subtract multiple of pivot row from current row,
            // such that pivot column element becomes 0
            float factor = copy[ row + n*pivot ];

            // subtract multiple of row
            for ( col = 0; col < n; ++col )
            {
                copy[ row + n*col ] -= factor*copy[ pivot + n*col ];   
            }
        }
    }

    delete [] copy;

    // invert to get final determinant
    return 1.0f/det;

}   // End of IvMatrix33::Determinant()
