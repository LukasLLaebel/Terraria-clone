#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <vector>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <blocks.h>

struct GameplayData
{
	glm::vec2 playerPos = { 100, 100 };
	glm::vec2 velocity = { 0, 0 };
	bool isOnGround = false;
	const glm::vec2 size = { 50, 100 };		// Witdh, Height
};

GameplayData data;

gl2d::Renderer2D renderer;

std::vector<Block> blocks;

bool initGame()
{
	gl2d::init();
	renderer.create();


	float blockStartY = 200.0f; // set starting height
	glm::vec2 blockSize = { 50.0f, 50.0f }; // Easily adjustable block size

	std::vector<BlockType> outline = {
		BlockType::Grass, BlockType::Water, BlockType::Water, BlockType::Water, BlockType::Grass, BlockType::Grass, BlockType::Grass,
		BlockType::Stone, BlockType::Water, BlockType::Water, BlockType::Water, BlockType::Stone, BlockType::Stone, BlockType::Stone,
		BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone,
		BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone, BlockType::Stone
	};

	int width = 7; // number of blocks per row
	int height = 4;
	//int height = static_cast<int>(outline.size()) / width;   // 2 layers.

	// Clear any existing blocks
	blocks.clear();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = y * width + x;
			glm::vec2 pos = {
				x * blockSize.x,
				blockStartY + y * blockSize.y
			};

			Block b(pos, outline[index]);
			b.size = blockSize; // set size of the block
			blocks.push_back(b);
		}
	}


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
