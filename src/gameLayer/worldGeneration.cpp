// worldGeneration.cpp
#include "worldGeneration.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

using std::min;
using std::max;

std::vector<Block> blocks;

const int worldWidth = 100;
const int worldHeight = 200;
const glm::vec2 blockSize = { 50.0f, 50.0f };
const float blockStartY = 200.0f;

BlockType getRandomBlockType()
{
	int r = rand() % 3;
	switch (r)
	{
	case 0: return BlockType::Grass;
	case 1: return BlockType::Water;
	default: return BlockType::Stone;
	}
}

// Helper functions for Perlin-like noise
float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float grad(int hash, float x) {
	return (hash & 1) == 0 ? x : -x;
}

float perlin1D(float x) {
	int xi = static_cast<int>(floor(x)) & 255;
	float xf = x - floor(x);
	float u = fade(xf);

	static int p[512];
	static bool initialized = false;
	if (!initialized) {
		for (int i = 0; i < 256; ++i) p[i] = i;
		for (int i = 0; i < 256; ++i) {
			int j = rand() % 256;
			std::swap(p[i], p[j]);
		}
		for (int i = 0; i < 256; ++i) p[256 + i] = p[i];
		initialized = true;
	}

	int aa = p[xi];
	int ab = p[xi + 1];

	return lerp(grad(aa, xf), grad(ab, xf - 1), u);
}



float grad2D(int hash, float x, float y) {
	switch (hash & 3) {
	case 0: return  x + y;
	case 1: return -x + y;
	case 2: return  x - y;
	case 3: return -x - y;
	default: return 0;
	}
}

float perlin2D(float x, float y) {
	static int p[512];
	static bool initialized = false;
	if (!initialized) {
		for (int i = 0; i < 256; ++i) p[i] = i;
		for (int i = 0; i < 256; ++i) std::swap(p[i], p[rand() % 256]);
		for (int i = 0; i < 256; ++i) p[256 + i] = p[i];
		initialized = true;
	}

	int xi = static_cast<int>(floor(x)) & 255;
	int yi = static_cast<int>(floor(y)) & 255;
	float xf = x - floor(x);
	float yf = y - floor(y);

	float u = fade(xf);
	float v = fade(yf);

	int aa = p[p[xi] + yi];
	int ab = p[p[xi] + yi + 1];
	int ba = p[p[xi + 1] + yi];
	int bb = p[p[xi + 1] + yi + 1];

	float x1 = lerp(grad2D(aa, xf, yf), grad2D(ba, xf - 1, yf), u);
	float x2 = lerp(grad2D(ab, xf, yf - 1), grad2D(bb, xf - 1, yf - 1), u);

	return lerp(x1, x2, v);
}

float fractalPerlin(float x, float y, int octaves, float persistence = 0.5f) {
	float total = 0, frequency = 1, amplitude = 1, maxValue = 0;
	for (int i = 0; i < octaves; ++i) {
		total += perlin2D(x * frequency, y * frequency) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2;
	}
	return total / maxValue;
}


void generateRandomWorld()
{
	blocks.clear();

	int baseGround = 100;
	int maxHillHeight = 10;

	float randomOffset = static_cast<float>(rand() % 100);

	std::vector<int> heightMap(worldWidth);
	float scale = 0.1f;

	for (int x = 0; x < worldWidth; ++x)
	{
		float noiseVal = perlin1D((x + randomOffset) * scale);
		int height = baseGround + static_cast<int>((noiseVal + 1.0f) * 0.5f * maxHillHeight);
		height = max(2, min(worldHeight - 5, height));
		heightMap[x] = height;
	}

	for (int x = 0; x < worldWidth; ++x)
	{
		int columnHeight = heightMap[x];
		bool waterCandidate = (columnHeight <= baseGround + 1);

		for (int y = 0; y < worldHeight; ++y)
		{
			glm::vec2 pos = {
				x * blockSize.x,
				blockStartY + (worldHeight - 1 - y) * blockSize.y
			};

			BlockType type;

			if (y < columnHeight - 3)
			{
				float nx = (x + randomOffset) * 0.08f;
				float ny = y * 0.08f;

				float caveNoise = fractalPerlin(nx, ny, 4, 0.5f);  // fractal noise
				float depthFactor = static_cast<float>(y) / worldHeight;
				float caveThreshold = 0.25f + depthFactor * 0.2f;

				if (caveNoise > caveThreshold)
				{
					continue; // empty block - part of cave
				}

				type = BlockType::Stone;
			}
			else if (y < columnHeight - 1)
			{
				type = BlockType::Stone;
			}
			else if (y == columnHeight - 1)
			{
				type = BlockType::Grass;
			}
			else
			{
				if (waterCandidate && y <= baseGround + 2 && x > 0 && x < worldWidth - 1)
				{
					if (heightMap[x - 1] <= baseGround + 1 && heightMap[x + 1] <= baseGround + 1)
					{
						type = BlockType::Water;
					}
					else continue;
				}
				else continue;
			}

			Block b(pos, type);
			b.size = blockSize;
			blocks.push_back(b);
		}
	}
	std::vector<int> treePositions;

	for (int x = 1; x < worldWidth - 1; ++x)
	{
		int y = heightMap[x] - 1;

		bool flatEnough = abs(heightMap[x - 1] - y) <= 1 && abs(heightMap[x + 1] - y) <= 1;
		bool hasSpace = std::all_of(treePositions.begin(), treePositions.end(), [&](int tx) {
			return abs(tx - x) >= 3;
			});

		if (flatEnough && hasSpace && rand() % 10 == 0)
		{
			generateTree(x, y);
			treePositions.push_back(x);
		}
	}
}


void generateTree(int x, int groundHeight)
{
	const int trunkHeight = 4 + rand() % 3;  // 4–6 blocks tall

	// Generate the trunk
	for (int i = 0; i < trunkHeight; ++i)
	{
		glm::vec2 pos = {
			x * blockSize.x,
			blockStartY + (worldHeight - 2 - (groundHeight + i)) * blockSize.y
		};
		blocks.push_back(Block(pos, BlockType::Wood));
	}

	int topY = groundHeight + trunkHeight;

	// Add a single leaf block at the top
	if (x >= 0 && x < worldWidth && topY >= 0 && topY < worldHeight)
	{
		glm::vec2 leafPos = {
			x * blockSize.x,
			blockStartY + (worldHeight - 1 - topY) * blockSize.y
		};
		blocks.push_back(Block(leafPos, BlockType::Leaves));
	}
}