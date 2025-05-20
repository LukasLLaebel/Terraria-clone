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
const int worldHeight = 100;
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

void generateRandomWorld()
{
	blocks.clear();

	int baseGround = 4;
	int maxHillHeight = 6;

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
				if ((x * y + rand() % 5) % 17 == 0)
				{
					continue; // cave
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
}
