//===============================================================================
// @ IvTeapot.cpp
// 
// Teapot rendering
// ------------------------------------------------------------------------------
// Copyright (C) 2008-2015 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// As you can see below, this is based on the original SGI teapot rendering code.
// We've replaced the OpenGL-specific Bezier surface rendering routines with our
// own.
//===============================================================================

/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 *
 * ALL RIGHTS RESERVED
 *
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Silicon
 * Graphics, Inc. not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
 * "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
 * OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
 * EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
 * ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
 * INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
 * SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
 * NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 *
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer
 * Software clause at DFARS 252.227-7013 and/or in similar or
 * successor clauses in the FAR or the DOD or NASA FAR
 * Supplement.  Unpublished-- rights reserved under the copyright
 * laws of the United States.  Contractor/manufacturer is Silicon
 * Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
 * 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "IvTeapot.h"

// 
// indices into Bezier patch data
//
static int patchdata[][16] =
{
    { 102, 103, 104, 105,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15 }, /* rim    */
    {  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27 }, /* body   */
    {  24,  25,  26,  27,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40 },
    {  96,  96,  96,  96,  97,  98,  99, 100, 101, 101, 101, 101,   0,   1,   2,   3 }, /* lid    */
    {   0,   1,   2,   3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117 },
    { 118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120,  40,  39,  38,  37 }, /* bottom */
    {  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56 }, /* handle */
    {  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  28,  65,  66,  67 },
    {  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83 }, /* spout  */
    {  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95 }
};

//
// The control points for all Bezier patches
//
static IvVector3 cpdata[] =
{
    IvVector3(0.2f, 0.0f, 2.7f), IvVector3(0.2f, -0.112f, 2.7f), IvVector3(0.112f, -0.2f, 2.7f), IvVector3(0.0f,
    -0.2f, 2.7f), IvVector3(1.3375f, 0.0f, 2.53125f), IvVector3(1.3375f, -0.749f, 2.53125f),
    IvVector3(0.749f, -1.3375f, 2.53125f), IvVector3(0.0f, -1.3375f, 2.53125f), IvVector3(1.4375f,
    0.0f, 2.53125f), IvVector3(1.4375f, -0.805f, 2.53125f), IvVector3(0.805f, -1.4375f,
    2.53125f), IvVector3(0.0f, -1.4375f, 2.53125f), IvVector3(1.5f, 0.0f, 2.4f), IvVector3(1.5f, -0.84f,
    2.4f), IvVector3(0.84f, -1.5f, 2.4f), IvVector3(0.0f, -1.5f, 2.4f), IvVector3(1.75f, 0.0f, 1.875f),
    IvVector3(1.75f, -0.98f, 1.875f), IvVector3(0.98f, -1.75f, 1.875f), IvVector3(0.0f, -1.75f,
    1.875f), IvVector3(2.0f, 0.0f, 1.35f), IvVector3(2.0f, -1.12f, 1.35f), IvVector3(1.12f, -2.0f, 1.35f),
    IvVector3(0.0f, -2.0f, 1.35f), IvVector3(2.0f, 0.0f, 0.9f), IvVector3(2.0f, -1.12f, 0.9f), IvVector3(1.12f, -2.0f,
    0.9f), IvVector3(0.0f, -2.0f, 0.9f), IvVector3(-2.0f, 0.0f, 0.9f), IvVector3(2.0f, 0.0f, 0.45f), IvVector3(2.0f, -1.12f,
    0.45f), IvVector3(1.12f, -2.0f, 0.45f), IvVector3(0.0f, -2.0f, 0.45f), IvVector3(1.5f, 0.0f, 0.225f),
    IvVector3(1.5f, -0.84f, 0.225f), IvVector3(0.84f, -1.5f, 0.225f), IvVector3(0.0f, -1.5f, 0.225f),
    IvVector3(1.5f, 0.0f, 0.15f), IvVector3(1.5f, -0.84f, 0.15f), IvVector3(0.84f, -1.5f, 0.15f), IvVector3(0.0f,
    -1.5f, 0.15f), IvVector3(-1.6f, 0.0f, 2.025f), IvVector3(-1.6f, -0.3f, 2.025f), IvVector3(-1.5f,
    -0.3f, 2.25f), IvVector3(-1.5f, 0.0f, 2.25f), IvVector3(-2.3f, 0.0f, 2.025f), IvVector3(-2.3f, -0.3f,
    2.025f), IvVector3(-2.5f, -0.3f, 2.25f), IvVector3(-2.5f, 0.0f, 2.25f), IvVector3(-2.7f, 0.0f,
    2.025f), IvVector3(-2.7f, -0.3f, 2.025f), IvVector3(-3.0f, -0.3f, 2.25f), IvVector3(-3.0f, 0.0f,
    2.25f), IvVector3(-2.7f, 0.0f, 1.8f), IvVector3(-2.7f, -0.3f, 1.8f), IvVector3(-3.0f, -0.3f, 1.8f),
    IvVector3(-3.0f, 0.0f, 1.8f), IvVector3(-2.7f, 0.0f, 1.575f), IvVector3(-2.7f, -0.3f, 1.575f), IvVector3(-3.0f,
    -0.3f, 1.35f), IvVector3(-3.0f, 0.0f, 1.35f), IvVector3(-2.5f, 0.0f, 1.125f), IvVector3(-2.5f, -0.3f,
    1.125f), IvVector3(-2.65f, -0.3f, 0.9375f), IvVector3(-2.65f, 0.0f, 0.9375f), IvVector3(-2.0f,
    -0.3f, 0.9f), IvVector3(-1.9f, -0.3f, 0.6f), IvVector3(-1.9f, 0.0f, 0.6f), IvVector3(1.7f, 0.0f,
    1.425f), IvVector3(1.7f, -0.66f, 1.425f), IvVector3(1.7f, -0.66f, 0.6f), IvVector3(1.7f, 0.0f,
    0.6f), IvVector3(2.6f, 0.0f, 1.425f), IvVector3(2.6f, -0.66f, 1.425f), IvVector3(3.1f, -0.66f,
    0.825f), IvVector3(3.1f, 0.0f, 0.825f), IvVector3(2.3f, 0.0f, 2.1f), IvVector3(2.3f, -0.25f, 2.1f),
    IvVector3(2.4f, -0.25f, 2.025f), IvVector3(2.4f, 0.0f, 2.025f), IvVector3(2.7f, 0.0f, 2.4f), IvVector3(2.7f,
    -0.25f, 2.4f), IvVector3(3.3f, -0.25f, 2.4f), IvVector3(3.3f, 0.0f, 2.4f), IvVector3(2.8f, 0.0f,
    2.475f), IvVector3(2.8f, -0.25f, 2.475f), IvVector3(3.525f, -0.25f, 2.49375f),
    IvVector3(3.525f, 0.0f, 2.49375f), IvVector3(2.9f, 0.0f, 2.475f), IvVector3(2.9f, -0.15f, 2.475f),
    IvVector3(3.45f, -0.15f, 2.5125f), IvVector3(3.45f, 0.0f, 2.5125f), IvVector3(2.8f, 0.0f, 2.4f),
    IvVector3(2.8f, -0.15f, 2.4f), IvVector3(3.2f, -0.15f, 2.4f), IvVector3(3.2f, 0.0f, 2.4f), IvVector3(0.0f, 0.0f,
    3.15f), IvVector3(0.8f, 0.0f, 3.15f), IvVector3(0.8f, -0.45f, 3.15f), IvVector3(0.45f, -0.8f,
    3.15f), IvVector3(0.0f, -0.8f, 3.15f), IvVector3(0.0f, 0.0f, 2.85f), IvVector3(1.4f, 0.0f, 2.4f), IvVector3(1.4f,
    -0.784f, 2.4f), IvVector3(0.784f, -1.4f, 2.4f), IvVector3(0.0f, -1.4f, 2.4f), IvVector3(0.4f, 0.0f,
    2.55f), IvVector3(0.4f, -0.224f, 2.55f), IvVector3(0.224f, -0.4f, 2.55f), IvVector3(0.0f, -0.4f,
    2.55f), IvVector3(1.3f, 0.0f, 2.55f), IvVector3(1.3f, -0.728f, 2.55f), IvVector3(0.728f, -1.3f,
    2.55f), IvVector3(0.0f, -1.3f, 2.55f), IvVector3(1.3f, 0.0f, 2.4f), IvVector3(1.3f, -0.728f, 2.4f),
    IvVector3(0.728f, -1.3f, 2.4f), IvVector3(0.0f, -1.3f, 2.4f), IvVector3(0.0f, 0.0f, 0.0f), IvVector3(1.425f,
    -0.798f, 0.0f), IvVector3(1.5f, 0.0f, 0.075f), IvVector3(1.425f, 0.0f, 0.0f), IvVector3(0.798f, -1.425f,
    0.0f), IvVector3(0.0f, -1.5f, 0.075f), IvVector3(0.0f, -1.425f, 0.0f), IvVector3(1.5f, -0.84f, 0.075f),
    IvVector3(0.84f, -1.5f, 0.075f)
};

//-------------------------------------------------------------------------------
// @ BuildMesh()
//-------------------------------------------------------------------------------
// This routine tessellates a 4x4 Bezier patch into a mesh, and stores it into the
// given dynamic arrays.  The idea is that we will use the four sets of control
// points in the 'v' direction to generate four Bezier curves.  We will then 
// tessellate along those curves to get the control points in the 'u' direction,
// which we will use to tessellate our final positions.
//
// Note that we also need to generate tangents in the 'u' and 'v' directions.  The
// 'u' direction tangents we can get from our 'u' tessellation.  The 'v' direction
// tangents we generate by creating a new set of Bezier control points.
//-------------------------------------------------------------------------------
static void 
BuildMesh( const IvVector3 controls[4][4], std::vector<IvTNPVertex>& vertices, std::vector<unsigned int>& indices )
{
    const unsigned int grid = 7;  
    const float h = 1.0f/(float)(grid-1);
    const float h2 = h*h;
    const float h3 = h2*h;

    unsigned int indexDiff = vertices.size();

    // set up forward differencing parameters for curves in v-direction
    IvVector3 v[4];         // position controls
    IvVector3 dv1[4];
    IvVector3 dv2[4];
    IvVector3 dv3[4];
    IvVector3 vtan[4];      // velocity controls (for v-direction)
    IvVector3 dvtan1[4];
    IvVector3 dvtan2[4];
    for ( unsigned int k = 0; k < 4; ++k )
    {
        IvVector3 A = controls[3][k]
                    - 3.0f*controls[2][k]
                    + 3.0f*controls[1][k]
                    - controls[0][k];
        IvVector3 B = 3.0f*controls[2][k]
                    - 6.0f*controls[1][k]
                    + 3.0f*controls[0][k];
        IvVector3 C = 3.0f*controls[1][k]
                    - 3.0f*controls[0][k];
        // scale down by 3/4
        A *= 0.75f;
        B *= 0.75f;
        C *= 0.75f;
        dv1[k] = h3*A + h2*B + h*C;
        dv3[k] = 6.0f*h3*A;
        dv2[k] = dv3[k] + 2.0f*h2*B;
        v[k] = 0.75f*controls[0][k];
        dvtan1[k] = 3.0f*h2*A + 2.0f*h*B;
        dvtan2[k] = 6.0f*h2*A;
        vtan[k] = C;
    }

    // start tessellating in the 'v' direction
    for ( unsigned int j = 0; j < grid; ++j )
    {
        // now build forward differencing variables for u-direction curve
        // using control points we've just tesselated
        IvVector3 A = v[3]
                    - 3.0f*v[2]
                    + 3.0f*v[1]
                    - v[0];
        IvVector3 B = 3.0f*v[2]
                    - 6.0f*v[1]
                    + 3.0f*v[0];
        IvVector3 C = 3.0f*v[1]
                    - 3.0f*v[0];
        IvVector3 dP1 = h3*A + h2*B + h*C;
        IvVector3 dP3 = 6.0f*h3*A;
        IvVector3 dP2 = dP3 + 2.0f*h2*B;
        IvVector3 P = v[0];
        IvVector3 ddu1 = 3.0f*h2*A + 2.0f*h*B;
        IvVector3 ddu2 = 6.0f*h2*A;
        IvVector3 du = C;

        // also need to forward difference v-velocities
        A = vtan[3]
            - 3.0f*vtan[2]
            + 3.0f*vtan[1]
            - vtan[0];
        B = 3.0f*vtan[2]
            - 6.0f*vtan[1]
            + 3.0f*vtan[0];
        C = 3.0f*vtan[1]
            - 3.0f*vtan[0];
        IvVector3 ddv1 = h3*A + h2*B + h*C;
        IvVector3 ddv3 = 6.0f*h3*A;
        IvVector3 ddv2 = ddv3 + 2.0f*h2*B;
        IvVector3 dv = vtan[0];

        // tessellate in the 'u' direction
        for ( unsigned int i = 0; i < grid; ++i )
        {
            IvTNPVertex vertex;
            vertex.position = P;
            IvVector2 coord( float(i)/float(grid-1), float(j)/float(grid-1) );
            vertex.texturecoord = coord;
            vertex.normal = Cross( dv, du );
            vertex.normal.Normalize();
           // whoops, singularity 
            if ( vertex.normal.IsZero() ) 
            { 
                // this is not a general solution, but it works for the teapot 
                // take average of face normals using previous row of vertices (if there is one) 
                if ( j > 0 ) 
                { 
                    vertex.normal.Zero(); 
                    for ( unsigned int k = 0; k < grid-1; ++k ) 
                    { 
                        unsigned int index0 = grid*(j-1) + k + indexDiff; 
                        unsigned int index1 = grid*(j-1) + k+1 + indexDiff; 
                        IvVector3 faceNormal = Cross(vertices[index0].position - P, 
                                                     vertices[index1].position - P); 
                        faceNormal.Normalize(); 
                        vertex.normal += faceNormal; 
                    } 
                    vertex.normal.Normalize(); 
                } 
            } 
 
            vertices.push_back( vertex );

            if ( i > 0 && j > 0 )
            {
                unsigned int index0 = grid*(j-1) + (i-1);
                unsigned int index1 = grid*(j-1) + i;
                unsigned int index2 = grid*j + (i-1);
                unsigned int index3 = grid*j + i;
                indices.push_back( index0 + indexDiff ); 
                indices.push_back( index2 + indexDiff  ); 
                indices.push_back( index3 + indexDiff  ); 
                indices.push_back( index0 + indexDiff  ); 
                indices.push_back( index3 + indexDiff  ); 
                indices.push_back( index1 + indexDiff  ); 
            }

            P += dP1;
            dP1 += dP2;
            dP2 += dP3;

            du += ddu1;
            ddu1 += ddu2;

            dv += ddv1;
            ddv1 += ddv2;
            ddv2 += ddv3;
        }

        // increment v-direction curves to get new control points
        for ( unsigned int k = 0; k < 4; ++k )
        {
            v[k] += dv1[k];
            dv1[k] += dv2[k];
            dv2[k] += dv3[k];
            vtan[k] += dvtan1[k];
            dvtan1[k] += dvtan2[k];
        }
    }
}

//-------------------------------------------------------------------------------
// @ BuildTeapot()
//-------------------------------------------------------------------------------
// This routine builds all the mesh data for the teapot, using the BuildMesh()
// routine above.  It cycles through all the patch data and reflects it to
// get a full mesh.
//-------------------------------------------------------------------------------
void 
BuildTeapot( std::vector<IvTNPVertex>& vertices, std::vector<unsigned int>& indices )
{
    IvVector3 p[4][4], q[4][4], r[4][4], s[4][4];

    for (unsigned int i = 0; i < 10; i++) 
    {
        for (unsigned int j = 0; j < 4; j++) 
        {
            for (unsigned int k = 0; k < 4; k++) {
                for (unsigned int l = 0; l < 3; l++) 
                {
                    p[3-j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
                    q[3-j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
                    if (l == 1)
                        q[3-j][k][l] *= -1.0;
                    if (i < 6) 
                    {
                        r[3-j][k][l] =
                        cpdata[patchdata[i][j * 4 + (3 - k)]][l];
                        if (l == 0)
                            r[3-j][k][l] *= -1.0;
                        s[3-j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
                        if (l == 0)
                            s[3-j][k][l] *= -1.0;
                        if (l == 1)
                            s[3-j][k][l] *= -1.0;
                    }
                }
            }
        }

        BuildMesh(p, vertices, indices);
        BuildMesh(q, vertices, indices);
        if ( i < 6 )
        {
            BuildMesh(r, vertices, indices);
            BuildMesh(s, vertices, indices);
        }
    }
}
