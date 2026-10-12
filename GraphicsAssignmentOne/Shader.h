#pragma once
#include "MyMath.h"

void (*VertexShader)(Float4&);
void (*PixelShader)(unsigned int&);
Matrix4x4 SV_WorldMatrix;
Matrix4x4 SV_ViewMatrix;
Matrix4x4 SV_ProjectionMatrix;

void VS_World(Float4& v)
{
    v = MatrixVertexMultiply(v, SV_WorldMatrix);
    v = MatrixVertexMultiply(v, SV_ViewMatrix);
    v = MatrixVertexMultiply(v, SV_ProjectionMatrix);

    // Perspective divide
    v.x /= v.w;
    v.y /= v.w;
    v.z /= v.w;
    v.w = 1.0f;
}

void PS_White(unsigned int& color) { color = 0xFFFFFFFF; }
void PS_Red(unsigned int& color) { color = 0xFFFF0000; }
void PS_Green(unsigned int& color) { color = 0xFF00FF00; }
void PS_Blue(unsigned int& color) { color = 0xFF0000FF; }