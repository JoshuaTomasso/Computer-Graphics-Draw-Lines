#pragma once
#include "Shader.h"
#include "fire_02.h"
#include "tiles_12.h"
#include "XTime.h"

unsigned int MyAlphaBlend(unsigned int DestinationColor, unsigned int SourceColor)
{
    unsigned int destA = (DestinationColor >> 24) & 0xFF;
    unsigned int destR = (DestinationColor >> 16) & 0xFF;
    unsigned int destG = (DestinationColor >> 8) & 0xFF;
    unsigned int destB = (DestinationColor >> 0) & 0xFF;

    unsigned int srcA = (SourceColor >> 24) & 0xFF;
    unsigned int srcR = (SourceColor >> 16) & 0xFF;
    unsigned int srcG = (SourceColor >> 8) & 0xFF;
    unsigned int srcB = (SourceColor >> 0) & 0xFF;

    float alpha = srcA / 255.0f;

    unsigned int outA = static_cast<unsigned int>(destA * (1.0f - alpha) + srcA * alpha);
    unsigned int outR = static_cast<unsigned int>(destR * (1.0f - alpha) + srcR * alpha);
    unsigned int outG = static_cast<unsigned int>(destG * (1.0f - alpha) + srcG * alpha);
    unsigned int outB = static_cast<unsigned int>(destB * (1.0f - alpha) + srcB * alpha);

    return (outA << 24) | (outR << 16) | (outG << 8) | outB;
}

// Pixel draw
void drawPixel(int x, int y, unsigned int color) 
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    int index = convertDimCoords(x, y);
    SCREEN_ARRAY[index] = MyAlphaBlend(SCREEN_ARRAY[index], color);
}

// Draw line using float interpolation
void DrawLine(const Float4WithColor& start, const Float4WithColor& end)
{
    // Copy input vertices
    Float4WithColor vStart = start;
    Float4WithColor vEnd = end;

    // Apply vertex shader if available
    if (VertexShader)
    {
        VertexShader(vStart.pos);
        VertexShader(vEnd.pos);
    }

    // Convert to screen space
    ScreenXY screenStart = NDCtoScreen(vStart.pos);
    ScreenXY screenEnd = NDCtoScreen(vEnd.pos);

    // Interpolation setup
    int dx = std::abs(screenEnd.x - screenStart.x);
    int dy = std::abs(screenEnd.y - screenStart.y);
    int steps = max(dx, dy);

    if (steps == 0)
    {
        unsigned int color = vStart.color;
        if (PixelShader)
            PixelShader(color);
        drawPixel(screenStart.x, screenStart.y, color);
        return;
    }


    float xStep = (screenEnd.x - screenStart.x) / static_cast<float>(steps);
    float yStep = (screenEnd.y - screenStart.y) / static_cast<float>(steps);

    float x = static_cast<float>(screenStart.x);
    float y = static_cast<float>(screenStart.y);

    for (int i = 0; i <= steps; ++i)
    {
        unsigned int color = vStart.color;
        if (PixelShader)
            PixelShader(color);
        drawPixel(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)), color);

        x += xStep;
        y += yStep;
    }
}


int RandomNumber100to400() 
{
    return rand() % 301 + 100;
}

void blockImageTransfer(int startX, int startY, int endX, int endY, int worldX, int worldY, const unsigned int* array, int arrayWidth) 
{
    for (int y = startY; y < endY; ++y)
    {
        for (int x = startX; x < endX; ++x)
        {
            int srcIndex = y * arrayWidth + x;

            if (worldX + (x - startX) < 0 || worldX + (x - startX) >= SCREEN_WIDTH ||
                worldY + (y - startY) < 0 || worldY + (y - startY) >= SCREEN_HEIGHT)
                continue;

            unsigned int color = C2C(array[srcIndex]);
            drawPixel(worldX + (x - startX), worldY + (y - startY), color);
        }
    }
}

void clearScreen() 
{
    for (int i = 0; i < NUM_PIXELS; ++i)
        SCREEN_ARRAY[i] = 0xFF000000; 
}

void LabOneStuff(XTime& timer, bool& treesInitialized, Tree* treeList, int numOfTrees, int& fireX, int& fireY, double& fireTimeAccumulator, const double fireFrameDuration)
{
    timer.Signal();
    fireTimeAccumulator += timer.Delta();

    if (fireTimeAccumulator >= fireFrameDuration) 
    {
        for (int y = 0; y < SCREEN_HEIGHT; y += 30)
        {
            for (int x = 0; x < SCREEN_WIDTH; x += 30)
            {
                blockImageTransfer(290, 130, 320, 160, x, y, tiles_12_pixels, tiles_12_width);
            }
        }

        if (!treesInitialized)
        {
            for (int i = 0; i < numOfTrees; i++)
            {
                treeList[i].x = RandomNumber100to400();
                treeList[i].y = RandomNumber100to400();
            }
            treesInitialized = true;
        }

        for (int i = 0; i < numOfTrees; i++) 
        {
            blockImageTransfer(320, 15, 385, 95, treeList[i].x, treeList[i].y, tiles_12_pixels, tiles_12_width);
        }

        blockImageTransfer(
            fireX, fireY,
            fireX + 128, fireY + 128,
            SCREEN_WIDTH / 2 - 64,
            SCREEN_HEIGHT / 2 - 64,
            fire_02_pixels,
            fire_02_width
        );

        fireTimeAccumulator -= fireFrameDuration;
        fireX += 128;
        if (fireX >= fire_02_width)
        {
            fireX = 0;
            fireY += 128;
            if (fireY >= fire_02_height)
                fireY = 0;
        }
    }
}

void LabTwoStuff()
{
    auto ScreenToNDC = [](int x, int y) -> Float4 {
        Float4 f;
        f.x = (2.0f * x / SCREEN_WIDTH) - 1.0f;
        f.y = 1.0f - (2.0f * y / SCREEN_HEIGHT); // invert Y
        f.z = 0.0f;
        f.w = 1.0f;
        return f;
        };

    auto MakeVertex = [&](int x, int y, unsigned int color) -> Float4WithColor {
        return { ScreenToNDC(x, y), color };
        };

    // Red star
    DrawLine(MakeVertex(356, 356, 0xffff0000), MakeVertex(250, 400, 0xffff0000));
    DrawLine(MakeVertex(144, 144, 0xffff0000), MakeVertex(250, 100, 0xffff0000));
    DrawLine(MakeVertex(400, 250, 0xffff0000), MakeVertex(356, 356, 0xffff0000));
    DrawLine(MakeVertex(100, 250, 0xffff0000), MakeVertex(144, 144, 0xffff0000));
    DrawLine(MakeVertex(250, 400, 0xffff0000), MakeVertex(144, 356, 0xffff0000));
    DrawLine(MakeVertex(250, 100, 0xffff0000), MakeVertex(356, 144, 0xffff0000));
    DrawLine(MakeVertex(144, 356, 0xffff0000), MakeVertex(100, 250, 0xffff0000));
    DrawLine(MakeVertex(356, 144, 0xffff0000), MakeVertex(400, 250, 0xffff0000));

    // Extra lines
    DrawLine(MakeVertex(250, 100, 0xff00ff00), MakeVertex(250, 400, 0xff00ff00)); // Green vertical
    DrawLine(MakeVertex(356, 356, 0xff0000ff), MakeVertex(144, 144, 0xff0000ff)); // Blue diagonal
    DrawLine(MakeVertex(144, 356, 0xff00ffff), MakeVertex(356, 144, 0xff00ffff)); // Cyan diagonal
    DrawLine(MakeVertex(400, 250, 0xffffff00), MakeVertex(100, 250, 0xffffff00)); // Yellow horizontal
}

void LabThreeStuff(XTime& timer, Float4* gridVerts, int gridVertCount, Float4* cubeVerts, int cubeVertCount)
{
    timer.Signal();
    float elapsed = static_cast<float>(timer.TotalTime());

    // === World Matrix for Grid ===
    Matrix4x4 gridOffset = CreateTranslationMatrix(0.0f, -0.5f, 1.0f);
    Matrix4x4 gridScale = CreateScaleMatrix(2.14f, 1.0f, 2.14f);
    Matrix4x4 worldGrid;
    MatrixMatrixMultiply(worldGrid, gridScale, gridOffset); 


    // === World Matrix for Cube ===
    Matrix4x4 rotationY = CreateRotationYMatrix(elapsed);
    Matrix4x4 translation = CreateTranslationMatrix(0.0f, 0.15f, 0.0f);
    Matrix4x4 worldCube;
    MatrixMatrixMultiply(worldCube, translation, rotationY);

    // === View Matrix ===
    Matrix4x4 viewRotation = CreateRotationXMatrix(18.0f * DEG2RAD);     // Tilt camera down
    Matrix4x4 viewTranslation = CreateTranslationMatrix(0.0f, 0.0f, -1.0f); // Pull back camera
    Matrix4x4 viewCombined;
    MatrixMatrixMultiply(viewCombined, viewTranslation, viewRotation);
    Matrix4x4 view = InverseRigidBodyMatrix(viewCombined); // Invert to get proper view matrix

    // === Projection Matrix ===
    Matrix4x4 proj = CreatePerspectiveMatrix(
        90.0f * DEG2RAD,
        static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT,
        0.1f,
        10.0f
    );

    VertexShader = VS_World;
    SV_ViewMatrix = view;
    SV_ProjectionMatrix = proj;

    // === DRAW GRID ===
    PixelShader = PS_White;
    SV_WorldMatrix = worldGrid;
    for (int i = 0; i < gridVertCount; i += 2)
    {
        Float4WithColor v0 = { gridVerts[i], 0xFFFFFFFF };
        Float4WithColor v1 = { gridVerts[i + 1], 0xFFFFFFFF };
        DrawLine(v0, v1);
    }

    // === DRAW CUBE ===
    PixelShader = PS_Green;
    SV_WorldMatrix = worldCube;
    for (int i = 0; i < cubeVertCount; i += 2)
    {
        Float4WithColor v0 = { cubeVerts[i], 0xFF00FF00 };
        Float4WithColor v1 = { cubeVerts[i + 1], 0xFF00FF00 };
        DrawLine(v0, v1);
    }

    PixelShader = nullptr;
}
