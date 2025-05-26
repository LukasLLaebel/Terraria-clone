#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "blocks.h"

extern std::vector<Block> blocks;

void generateRandomWorld();
void generateTree(int x, int groundHeight);