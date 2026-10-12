#include "RasterSurface.h"
#include "tiles_12.h"
#include "fire_02.h"
#include "XTime.h"
#include <cmath>
#include <random>
#include <time.h>


#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define NUM_PIXELS (SCREEN_WIDTH * SCREEN_HEIGHT)
#define C2C(c) (((c&65280u)<<0x8u)|((c&4278190080u)>>0x18u)|((c&255u)<<0x18u)|((c&16711680u)>>0x08u))

unsigned int SCREEN_ARRAY[NUM_PIXELS] = { 0xff000000, };

XTime timer;

const int numOfTrees = 10;

const int fireCols = 8;
const int fireRows = 8;
const int fireWidth = 1024;
const int fireHeight = 1024;
const int fireCellWidth = fireWidth / fireCols;
const int fireCellHeight = fireHeight / fireRows;

int fireX = 0;
int fireY = 0;
double fireTimeAccumulator = 0.0;
const double fireFrameDuration = 1.0 / 30.0;

void clearScreen();
int convertDimCoords(int xPos, int yPos);
void drawPixel(int xPos, int yPos, unsigned int color);
void blockImageTransfer(int startX, int startY, int endX, int endY, int worldX, int worldY, const unsigned int* array, int arrayWidth);
unsigned int AlphaBlend(unsigned int DestinationColor, unsigned int SourceColor);
void drawLine(int x1, int y1, int x2, int y2, unsigned int color);
void LabOneStuff();
void LabTwoStuff();
unsigned int RandomNumber100to400();

struct Tree {
	int x;
	int y;
};

Tree treeList[numOfTrees];
bool treesInitialized = false;

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	RS_Initialize("Joshua Tomasso", SCREEN_WIDTH, SCREEN_HEIGHT);
	timer.Restart();
	clearScreen();
	do {

		//LabOneStuff();
		LabTwoStuff();

	} while (RS_Update(SCREEN_ARRAY, NUM_PIXELS));

	RS_Shutdown();
	return 0;

}

void clearScreen()
{
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		SCREEN_ARRAY[i] = 0xff000000;
	}
}

int convertDimCoords(int xPos, int yPos)
{
	return yPos * SCREEN_WIDTH + xPos;
}

void drawPixel(int xPos, int yPos, unsigned int color)
{
	if (xPos < 0 || xPos >= SCREEN_WIDTH || yPos < 0 || yPos >= SCREEN_HEIGHT)
		return;

	int arrayPos = convertDimCoords(xPos, yPos);
	SCREEN_ARRAY[arrayPos] = AlphaBlend(SCREEN_ARRAY[arrayPos], color);
}

void blockImageTransfer(int startX, int startY, int endX, int endY, int worldX, int worldY, const unsigned int* array, int arrayWidth)
{
	for (int y = startY; y < endY; y++)
	{
		for (int x = startX; x < endX; x++)
		{
			int srcIndex = y * arrayWidth + x;

			if (worldX + (x - startX) < 0 || worldX + (x - startX) >= SCREEN_WIDTH || worldY + (y - startY) < 0 || worldY + (y - startY) >= SCREEN_HEIGHT)
				continue;

			unsigned int color = C2C(array[srcIndex]);
			drawPixel(worldX + (x - startX), worldY + (y - startY), color);
		}
	}
}

unsigned int AlphaBlend(unsigned int DestinationColor, unsigned int SourceColor)
{
	// Extract RGBA components from both colors
	unsigned int destA = (DestinationColor >> 24) & 0xFF;
	unsigned int destR = (DestinationColor >> 16) & 0xFF;
	unsigned int destG = (DestinationColor >> 8) & 0xFF;
	unsigned int destB = (DestinationColor >> 0) & 0xFF;

	unsigned int srcA = (SourceColor >> 24) & 0xFF;
	unsigned int srcR = (SourceColor >> 16) & 0xFF;
	unsigned int srcG = (SourceColor >> 8) & 0xFF;
	unsigned int srcB = (SourceColor >> 0) & 0xFF;

	// Calculate blending ratio from source alpha
	float fAlphaRatio = srcA / 255.0f;

	// Interpolate each color channel
	unsigned int outA = static_cast<unsigned int>(destA * (1.0f - fAlphaRatio) + srcA * fAlphaRatio);
	unsigned int outR = static_cast<unsigned int>(destR * (1.0f - fAlphaRatio) + srcR * fAlphaRatio);
	unsigned int outG = static_cast<unsigned int>(destG * (1.0f - fAlphaRatio) + srcG * fAlphaRatio);
	unsigned int outB = static_cast<unsigned int>(destB * (1.0f - fAlphaRatio) + srcB * fAlphaRatio);

	// Recombine into single 32-bit color (ARGB)
	return (outA << 24) | (outR << 16) | (outG << 8) | (outB);
}

void drawLine(int x1, int y1, int x2, int y2, unsigned int color)
{
	float dx = static_cast<float>(x2 - x1);
	float dy = static_cast<float>(y2 - y1);
	
	float length = std::sqrt(dx * dx + dy * dy);
	if (length == 0.0f)
	{
		drawPixel((int)x1, (int)y1, color); 
		return;
	}
	
	float step = 1.0f / length;
	float t = 0.0f;
	while (t <= 1.0f)
	{
		float x = x1 + t * dx;
		float y = y1 + t * dy;

		drawPixel((int)x, (int)y, color);

		t += step;
	}
}

void LabOneStuff()
{
		//drawPixel(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0xffffffff);
		

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
				fireX + fireCellWidth, fireY + fireCellHeight,
				SCREEN_WIDTH / 2 - fireCellWidth / 2,
				SCREEN_HEIGHT / 2 - fireCellHeight / 2,
				fire_02_pixels,
				fireWidth
			);
			fireTimeAccumulator -= fireFrameDuration;

			fireX += fireCellWidth;
			if (fireX >= fireWidth)
			{
				fireX = 0;
				fireY += fireCellHeight;
				if (fireY >= fireHeight)
					fireY = 0;
			}
		}

}

unsigned int RandomNumber100to400()
{
	
	
	return rand() % 301 + 100;
	
}

void LabTwoStuff()
{
	

	drawLine(356, 356, 250, 400, 0xffff0000);
	drawLine(144, 144, 250, 100, 0xffff0000);
	drawLine(400, 250, 356, 356, 0xffff0000);
	drawLine(100, 250, 144, 144, 0xffff0000);
	drawLine(250, 400, 144, 356, 0xffff0000);
	drawLine(250, 100, 356, 144, 0xffff0000);
	drawLine(144, 356, 100, 250, 0xffff0000);
	drawLine(356, 144, 400, 250, 0xffff0000);


	drawLine(250, 100, 250, 400, 0xff00ff00);
	drawLine(356, 356, 144, 144, 0xff0000ff);
	drawLine(144, 356, 356, 144, 0xff00ffff);
	drawLine(400, 250, 100, 250, 0xffffff00);
	
}