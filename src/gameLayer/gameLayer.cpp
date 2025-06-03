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
#include <worldGeneration.h>
#include "UI.h"
#include <entities.h>
#include <utilities.h>

using std::min;
using std::max;

struct GameplayData
{
	glm::vec2 playerPos = { 100, 100 };
	glm::vec2 velocity = { 0, 0 };
	bool isOnGround = false;
	const glm::vec2 size = { 50, 100 };		// Witdh, Height

}data;


//enum class FacingDirection { Left, Right };
FacingDirection playerFacing = FacingDirection::Right;

gl2d::Font font;
gl2d::Renderer2D renderer;

gl2d::Texture playerTexture;
gl2d::Texture bunnyTexture;
//gl2d::Texture bunnyAtlasTexture;
gl2d::Texture hedgehogTexture;

gl2d::Texture pickaxeTexture;
gl2d::Texture axeTexture;
gl2d::Texture swordTexture;

gl2d::Texture dirtTexture;
gl2d::Texture cobblestroneTexture;
gl2d::Texture woodTexture;
gl2d::Texture LeavesTexture;
gl2d::Texture tempTexture;


//gl2d::Texture backgroundTexture;

bool initGame()
{
	gl2d::init();
	renderer.create();

	font.createFromFile(RESOURCES_PATH "font/ANDYB.TTF");
	
	playerTexture.loadFromFile(RESOURCES_PATH "entities/entity/Trent.png", true);
	bunnyTexture.loadFromFile(RESOURCES_PATH "entities/entity/bunny.png", true);
	hedgehogTexture.loadFromFile(RESOURCES_PATH "entities/entity/hedgehog.png", true);

	//bunnyAtlasTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "entities/stitchedFiles/bunnySprite.png", 128, true);
	//gl2d::TextureAtlasPadding bunnyAtlas(8, 1, bunnyAtlasTexture.GetSize().x, bunnyAtlasTexture.GetSize().y);

	pickaxeTexture.loadFromFile(RESOURCES_PATH "tools/copperpickaxe.png", true);
	axeTexture.loadFromFile(RESOURCES_PATH "tools/copperaxe.png", true);
	swordTexture.loadFromFile(RESOURCES_PATH "tools/coppersword.png", true);

	//grassTexture.loadFromFile(RESOURCES_PATH "blocks/grassblock.png", true);
	dirtTexture.loadFromFile(RESOURCES_PATH "blocks/dirtblock.png", true);
	cobblestroneTexture.loadFromFile(RESOURCES_PATH "blocks/stoneblock.png", true);
	woodTexture.loadFromFile(RESOURCES_PATH "blocks/wood.png", true);
	LeavesTexture.loadFromFile(RESOURCES_PATH "blocks/treeTop.png", true);
	tempTexture.loadFromFile(RESOURCES_PATH "blocks/tempBlock.png", true);

	srand(static_cast<unsigned>(time(0))); // seed for random numbers
	generateRandomWorld(); // generate the initial random world

	//backgroundTexture.loadFromFile(RESOURCES_PATH "background/forrestBGMain.png", true);
	
	// find this in UI
	startItemsInventory();

	return true;
}

bool gameLogic(float deltaTime)
{
	//static float fps = 0.0f;
	//static float timeAccumulator = 0.0f;
	//static int frameCount = 0;

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

	// find this in UI
	handleInventoryInput();

	//bunnySpawn(deltaTime,data.playerPos);
	//hedgehogSpawn(deltaTime, data.playerPos);

#pragma region Movement
	const float gravity = 500.0f;
	const float jumpVelocity = -300.0f;
	bool facingRight = true;

	// Apply gravity
	data.velocity.y += gravity * deltaTime;

	// Horizontal movement
	if (platform::isButtonHeld(platform::Button::A) || platform::isButtonHeld(platform::Button::Left))
	{
		data.velocity.x = -200.0f;
		playerFacing = FacingDirection::Left;
	}
	else if (platform::isButtonHeld(platform::Button::D) || platform::isButtonHeld(platform::Button::Right))
	{
		data.velocity.x = 200.0f;
		playerFacing = FacingDirection::Right;
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
	
#pragma endregion


	// player collision (find this in utilities)
	Collision(data.playerPos, data.isOnGround, data.size, data.velocity);

	//bunnyLogic(deltaTime, gravity, data.playerPos, jumpVelocity, blocks);

	//hedgehogLogic(deltaTime, gravity, data.playerPos, jumpVelocity, blocks);

#pragma region Mouse Interaction
	// Get the current mouse position relative to the window (screen space)
	glm::vec2 screenMouse = platform::getRelMousePosition();

	// Convert the screen space mouse position to world space
	// This accounts for the camera's current position (scroll offset)
	glm::vec2 worldMouse = glm::vec2(screenMouse) + renderer.currentCamera.position;

	// Snap the mouse position to the nearest 50x50 grid cell
	glm::vec2 snappedMouse = glm::floor(worldMouse / 50.0f) * 50.0f;

	InventorySlot& slot = getSelectedSlot();
	
	// find these in utilities
	leftMouse(snappedMouse, slot);
	rightMouse(snappedMouse, data.playerPos);
	
#pragma endregion
	/*
	// For debuging
	// FPS calculater
	timeAccumulator += deltaTime;
	frameCount++;

	if (timeAccumulator >= 1.0f)
	{
		fps = frameCount / timeAccumulator;
		timeAccumulator = 0.0f;
		frameCount = 0;
	}
	*/
#pragma region Rendering
	// Render all blocks
	for (auto& block : blocks)
	{
		block.render(renderer);
	}

	// Render player
	glm::vec2 drawPos = data.playerPos;
	glm::vec2 drawSize = data.size;

	// Flip the sprite horizontally if facing left
	if (playerFacing == FacingDirection::Left)
	{
		drawPos.x += drawSize.x; // move origin to the right side
		drawSize.x *= -1;         // flip horizontally
	}

	renderer.renderRectangle({ drawPos, drawSize }, playerTexture);

	
	for (auto& bunny : bunnies)
	{
		// render bunny
		glm::vec2 bunnyPos = bunny.position;
		glm::vec2 bunnySize = bunny.Size;
		// Flip the sprite horizontally if facing left
		if (bunny.facing == FacingDirection::Right)
		{
			bunnyPos.x += bunnySize.x; // move origin to the right side
			bunnySize.x *= -1;         // flip horizontally
		}

		renderer.renderRectangle({ bunnyPos, bunnySize }, bunnyTexture);
	}

	// render hedgehog
	for (auto& hedgehog : hedgehogs)
	{
		// render hedgehog
		glm::vec2 hedgehogPos = hedgehog.position;
		glm::vec2 hedgehogSize = hedgehog.Size;
		// Flip the sprite horizontally if facing left
		if (hedgehog.facing == FacingDirection::Left)
		{
			hedgehogPos.x += hedgehogSize.x; // move origin to the right side
			hedgehogSize.x *= -1;         // flip horizontally
		}

		renderer.renderRectangle({ hedgehogPos, hedgehogSize }, hedgehogTexture);
	}




	// draw tools
	// Render the tool in player's hand
	InventorySlot& heldSlot = inventory[selectedSlot];
	if (heldSlot.occupied && heldSlot.isTool)
	{
		gl2d::Texture* toolTex = nullptr;

		switch (static_cast<ToolType>(heldSlot.itemID))
		{
		case ToolType::Pickaxe:
			toolTex = &pickaxeTexture;
			break;
		case ToolType::Axe:
			toolTex = &axeTexture;
			break;
		case ToolType::Sword:
			toolTex = &swordTexture;
			break;
		default:
			break;
		}

		if (toolTex)
		{
			glm::vec2 toolSize = { 50, 50 };
			glm::vec2 offset;

			if (playerFacing == FacingDirection::Right)
			{
				offset = glm::vec2(30, 30);
				renderer.renderRectangle({ data.playerPos + offset, toolSize }, *toolTex);
			}
			else
			{
				offset = glm::vec2(-30, 30);
				toolSize.x *= -1; // flip horizontally
				renderer.renderRectangle({ data.playerPos + offset + glm::vec2(data.size.x, 0), toolSize }, *toolTex);
			}
		}
	}





	// Render block preview
	renderer.renderRectangle({ snappedMouse, 50, 50}, Colors_Red); // draws outline only


#pragma region UI

	glm::vec2 fullInventorySize = glm::vec2(
		inventoryCols * (slotSize + slotPadding) - slotPadding,
		totalInventoryRows * (slotSize + slotPadding) - slotPadding
	);

	glm::vec2 startPos = renderer.currentCamera.position
		+ glm::vec2(platform::getFrameBufferSizeX(), 0)
		- glm::vec2(fullInventorySize.x, 0)
		- glm::vec2(20, -20); // padding: right and top

	// ---- Draw top (persistent) row ----
	for (int col = 0; col < inventoryCols; ++col)
	{
		int index = col; // Top row is the first 'inventoryCols' slots
		glm::vec2 pos = startPos + glm::vec2(col * (slotSize + slotPadding), 0);

		renderer.renderRectangle({ pos, slotSize, slotSize }, Colors_Gray);
		// Highlight selected slot
		if (selectedSlot == col)
		{
			renderer.renderRectangleOutline({ pos, slotSize, slotSize }, Colors_Yellow, 3.0f); // outline with 3px thickness
		}

		inventoryOccupied(renderer, index, font, pos);
	}

	// ---- Draw 4 bottom rows ----
	if (inventoryOpen)
	{
		for (int row = 0; row < inventoryRows; ++row)
		{
			for (int col = 0; col < inventoryCols; ++col)
			{
				int index = (persistentTopRows + row) * inventoryCols + col;
				glm::vec2 pos = startPos + glm::vec2(col * (slotSize + slotPadding), (row + 1) * (slotSize + slotPadding));

				renderer.renderRectangle({ pos, slotSize, slotSize }, Colors_Gray);

				inventoryOccupied(renderer, index, font, pos);
			}
		}
	}

#pragma endregion	



	renderer.flush();
#pragma endregion

	/* Debug menu
	ImGui::Begin("Game Controls | Debug");
	ImGui::Text("FPS: %.1f", fps);
	ImGui::End();
	*/

	return true;
}

void closeGame()
{
	// Cleanup if needed
}
