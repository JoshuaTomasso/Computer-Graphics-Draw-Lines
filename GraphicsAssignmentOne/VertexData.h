#pragma once
#include "Defines.h"

// Grid and cube vertex arrays 
#ifndef GRID_CUBE_VERTS_DEFINED
#define GRID_CUBE_VERTS_DEFINED

// 22 lines x 2 = 44 vertices
Float4 gridVerts[44];

// 12 edges x 2 = 24 vertices
Float4 cubeVerts[24];

#endif // GRID_CUBE_VERTS_DEFINED

// === Initializer Functions ===
void InitializeGridVerts() 
{
    int index = 0;
    for (int i = 0; i <= 10; ++i) 
    {
        float t = -0.5f + i * (1.0f / 10.0f);
        // Horizontal line
        gridVerts[index++] = { -0.5f, 0.0f, t, 1.0f };
        gridVerts[index++] = { 0.5f, 0.0f, t, 1.0f };
        // Vertical line
        gridVerts[index++] = { t, 0.0f, -0.5f, 1.0f };
        gridVerts[index++] = { t, 0.0f,  0.5f, 1.0f };
    }
}

void InitializeCubeVerts() 
{
    float h = 0.25f;
    Float4 corners[8] = {
        {-h, -h, -h, 1.0f}, { h, -h, -h, 1.0f},
        { h, -h,  h, 1.0f}, {-h, -h,  h, 1.0f},
        {-h,  h, -h, 1.0f}, { h,  h, -h, 1.0f},
        { h,  h,  h, 1.0f}, {-h,  h,  h, 1.0f}
    };

    int edgePairs[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    for (int i = 0; i < 12; ++i) 
    {
        cubeVerts[i * 2 + 0] = corners[edgePairs[i][0]];
        cubeVerts[i * 2 + 1] = corners[edgePairs[i][1]];
    }
}
