#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <vector>
#include <cstdlib> // For rand()
#include <ctime>   // For time()
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <blocks.h>

using std::min;
using std::max;

struct GameplayData
{
	glm::vec2 playerPos = { 100, 100 };
	glm::vec2 velocity = { 0, 0 };
	bool isOnGround = false;
	const glm::vec2 size = { 50, 100 };		// Witdh, Height

};

GameplayData data;

gl2d::Renderer2D renderer;

//gl2d::Texture backgroundTexture;

std::vector<Block> blocks;

const int worldWidth = 200;
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

void generateRandomWorld()
{
	blocks.clear();

	int baseGround = 4; // base ground layer
	int maxHillHeight = 6;

	// here i made a change : "random offset for more variety"
	float randomOffset = static_cast<float>(rand() % 100);

	// Generate a height map with sinusoidal hills and some noise
	std::vector<int> heightMap(worldWidth);
	for (int x = 0; x < worldWidth; ++x)
	{
		float hill = sinf((x + randomOffset) * 0.2f) * maxHillHeight; // hill shape
		float noise = (rand() % 3 - 1); // -1, 0, or +1 random variation
		int height = baseGround + static_cast<int>(hill + noise);
		height = max(2, min(worldHeight - 5, height)); // clamp to avoid edge errors
		heightMap[x] = height;
	}

	for (int x = 0; x < worldWidth; ++x)
	{
		int columnHeight = heightMap[x];
		bool waterCandidate = (columnHeight <= baseGround + 1); // lowland

		for (int y = 0; y < worldHeight; ++y)
		{
			glm::vec2 pos = {
				x * blockSize.x,
				blockStartY + (worldHeight - 1 - y) * blockSize.y
			};

			BlockType type;

			if (y < columnHeight - 3)
			{
				// here i made a change : "random caves with noise-like pattern"
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
				// Air or water above ground
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

bool initGame()
{
	gl2d::init();
	renderer.create();


	srand(static_cast<unsigned>(time(0))); // seed for random numbers
	generateRandomWorld(); // generate the initial random world

	//backgroundTexture.loadFromFile(RESOURCES_PATH "background/forrestBGMain.png", true);
	return true;
}

bool gameLogic(float deltaTime)
{
#pragma region Init
	int w = platform::getFrameBufferSizeX();
	int h = platform::getFrameBufferSizeY();

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.updateWindowMetrics(w, h);
#pragma endregion

	if (platform::isButtonPressedOn(platform::Button::G))
	{
		generateRandomWorld(); // regenerate random world on G key press
	}


#pragma region Movement
	const float gravity = 500.0f;
	const float jumpVelocity = -300.0f;

	// Apply gravity
	data.velocity.y += gravity * deltaTime;

	// Horizontal movement
	if (platform::isButtonHeld(platform::Button::A) || platform::isButtonHeld(platform::Button::Left))
	{
		data.velocity.x = -200.0f;
	}
	else if (platform::isButtonHeld(platform::Button::D) || platform::isButtonHeld(platform::Button::Right))
	{
		data.velocity.x = 200.0f;
	}
	else
	{
		data.velocity.x = 0.0f;
	}

	// Jumping
	if (platform::isButtonPressedOn(platform::Button::Space) && data.isOnGround)
	{
		data.velocity.y = jumpVelocity;
		data.isOnGround = false;
	}

	// Update position
	data.playerPos += data.velocity * deltaTime;

#pragma endregion




#pragma region camera follow
	glm::vec2 cameraTarget = data.playerPos + data.size * 0.5f;
	renderer.currentCamera.position = cameraTarget - glm::vec2(w, h) * 0.5f;
	
	//renderer.currentCamera.follow(data.playerPos, deltaTime * 550, 10, 50, w, h);
#pragma endregion


#pragma region Collision
	data.isOnGround = false;

	glm::vec4 playerRect = { data.playerPos.x, data.playerPos.y, data.size.x, data.size.y };

	for (auto& block : blocks)
	{
		glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

		bool isTouching =
			playerRect.x < blockRect.x + blockRect.z &&
			playerRect.x + playerRect.z > blockRect.x &&
			playerRect.y + playerRect.w > blockRect.y &&
			playerRect.y + playerRect.w < blockRect.y + blockRect.w;

		if (isTouching)
		{
			data.playerPos.y = block.position.y - data.size.y;
			data.velocity.y = 0;
			data.isOnGround = true;
			break;
		}
	}
#pragma endregion

#pragma region Mouse Interaction
	// Get the current mouse position relative to the window (screen space)
	glm::vec2 screenMouse = platform::getRelMousePosition();

	// Convert the screen space mouse position to world space
	// This accounts for the camera's current position (scroll offset)
	glm::vec2 worldMouse = glm::vec2(screenMouse) + renderer.currentCamera.position;

	// Snap the mouse position to the nearest 50x50 grid cell
	glm::vec2 snappedMouse = glm::floor(worldMouse / 50.0f) * 50.0f;

	// Place a new block at the snapped position if left mouse is pressed
	if (platform::isLMousePressed())
	{
		bool exists = false;

		// Check if a block already exists at this position
		for (auto& b : blocks)
		{
			if (b.position == snappedMouse)
			{
				exists = true;
				break;
			}
		}

		// Only add the block if no existing block was found
		if (!exists)
		{
			blocks.emplace_back(snappedMouse, BlockType::Stone);
		}
	}

	// Destroy the block at the snapped position if right mouse is pressed
	if (platform::isRMousePressed())
	{
		for (int i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i].position == snappedMouse)
			{
				// Remove the first block that matches the snapped position
				blocks.erase(blocks.begin() + i);
				break;
			}
		}
	}

#pragma endregion

#pragma region Rendering
	// Render all blocks
	for (auto& block : blocks)
	{
		block.render(renderer);
	}

	// Render player
	renderer.renderRectangle({ data.playerPos, data.size }, Colors_White);

	// Render block preview
	renderer.renderRectangle({ snappedMouse, 50, 50}, Colors_Red); // draws outline only


	renderer.flush();
#pragma endregion

	return true;
}

void closeGame()
{
	// Cleanup if needed
}
