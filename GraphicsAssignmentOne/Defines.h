#pragma once

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define NUM_PIXELS (SCREEN_WIDTH * SCREEN_HEIGHT)

// Color conversion macro (BGRA → ARGB)
#define C2C(c) (((c & 0x0000FF00u) << 8u) | ((c & 0xFF000000u) >> 24u) | ((c & 0x000000FFu) << 24u) | ((c & 0x00FF0000u) >> 8u))

// Global draw buffer
unsigned int SCREEN_ARRAY[NUM_PIXELS];

// Structs
struct Float4 
{
    float x, y, z, w;
};

struct Float4WithColor
{
    Float4 pos;           // Position in 4D space
    unsigned int color;   // Color in ARGB format
};

struct Matrix4x4 
{
    float m[4][4];
};

struct ScreenXY
{
    int x, y;
};

struct Tree
{
    int x;
    int y;
};

