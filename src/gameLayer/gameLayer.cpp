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
//#include <inventory.h>
#include <entities.h>

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

// Inventory structure and state
constexpr int inventoryCols = 10;
constexpr int totalInventoryRows = 5; // total including top row
constexpr int persistentTopRows = 1;
constexpr int inventoryRows = totalInventoryRows - persistentTopRows; // for toggled section
constexpr int slotSize = 50;
constexpr int slotPadding = 4;
int selectedSlot = 0; // 0–9 (top inventory row)

bool inventoryOpen = false;


enum class ToolType {
	None = -1,
	Pickaxe = 100,
	Axe,
	Sword
};

struct InventorySlot
{
	bool occupied = false;
	int itemID = -1;
	int count = 0;
	bool isTool = false;
};



std::vector<InventorySlot> inventory(inventoryCols* totalInventoryRows);
gl2d::Font font;
gl2d::Renderer2D renderer;

gl2d::Texture playerTexture;
gl2d::Texture bunnyTexture;
//gl2d::Texture bunnyAtlasTexture;


gl2d::Texture pickaxeTexture;
gl2d::Texture axeTexture;
gl2d::Texture swordTexture;

gl2d::Texture dirtTexture;
gl2d::Texture cobblestroneTexture;
//gl2d::Texture grassTexture;
gl2d::Texture tempTexture;


//gl2d::Texture backgroundTexture;

bool initGame()
{
	gl2d::init();
	renderer.create();

	font.createFromFile(RESOURCES_PATH "font/ANDYB.TTF");
	
	playerTexture.loadFromFile(RESOURCES_PATH "entities/entity/Trent.png", true);
	bunnyTexture.loadFromFile(RESOURCES_PATH "entities/entity/bunny.png", true);
	
	//bunnyAtlasTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "entities/stitchedFiles/bunnySprite.png", 128, true);
	//gl2d::TextureAtlasPadding bunnyAtlas(8, 1, bunnyAtlasTexture.GetSize().x, bunnyAtlasTexture.GetSize().y);

	pickaxeTexture.loadFromFile(RESOURCES_PATH "tools/copperpickaxe.png", true);
	axeTexture.loadFromFile(RESOURCES_PATH "tools/copperaxe.png", true);
	swordTexture.loadFromFile(RESOURCES_PATH "tools/coppersword.png", true);

	//grassTexture.loadFromFile(RESOURCES_PATH "blocks/grassblock.png", true);
	dirtTexture.loadFromFile(RESOURCES_PATH "blocks/dirtblock.png", true);
	cobblestroneTexture.loadFromFile(RESOURCES_PATH "blocks/stoneblock.png", true);
	tempTexture.loadFromFile(RESOURCES_PATH "blocks/tempBlock.png", true);

	srand(static_cast<unsigned>(time(0))); // seed for random numbers
	generateRandomWorld(); // generate the initial random world

	//backgroundTexture.loadFromFile(RESOURCES_PATH "background/forrestBGMain.png", true);
	
	// items
	inventory[0] = { true, static_cast<int>(ToolType::Pickaxe), 1, true };
	inventory[1] = { true, static_cast<int>(ToolType::Axe), 1, true };
	inventory[2] = { true, static_cast<int>(ToolType::Sword), 1, true };


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
	// Toggle inventory
	if (platform::isButtonPressedOn(platform::Button::Tab))
	{
		inventoryOpen = !inventoryOpen;
	}


	for (int i = 0; i < 9; ++i)
	{
		if (platform::isButtonPressedOn(platform::Button::NR1 + i % 9))
		{
			selectedSlot = i; // 0 to 8
			break;
		}
		if (platform::isButtonPressedOn(platform::Button::NR0)) {
			selectedSlot = 9;
			break;
		}
	}

	bunnySpawn(deltaTime,data.playerPos);

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


#pragma region Collision
	data.isOnGround = false;

	glm::vec4 playerRect = { data.playerPos.x, data.playerPos.y, data.size.x, data.size.y };

	for (auto& block : blocks)
	{
		glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

		bool xOverlap = playerRect.x < blockRect.x + blockRect.z &&
			playerRect.x + playerRect.z > blockRect.x;
		bool yOverlap = playerRect.y < blockRect.y + blockRect.w &&
			playerRect.y + playerRect.w > blockRect.y;

		if (xOverlap && yOverlap)
		{
			// Calculate penetration depths
			float fromLeft = (playerRect.x + playerRect.z) - blockRect.x;
			float fromRight = (blockRect.x + blockRect.z) - playerRect.x;
			float fromTop = (playerRect.y + playerRect.w) - blockRect.y;
			float fromBottom = (blockRect.y + blockRect.w) - playerRect.y;

			float minHoriz = min(fromLeft, fromRight);
			float minVert = min(fromTop, fromBottom);

			if (minHoriz < minVert)
			{
				// Horizontal collision
				if (fromLeft < fromRight)
				{
					// Colliding from left
					data.playerPos.x = blockRect.x - data.size.x;
				}
				else
				{
					// Colliding from right
					data.playerPos.x = blockRect.x + blockRect.z;
				}
				data.velocity.x = 0;
			}
			else
			{
				// Vertical collision
				if (fromTop < fromBottom)
				{
					// Colliding from above (landing)
					data.playerPos.y = blockRect.y - data.size.y;
					data.velocity.y = 0;
					data.isOnGround = true;
				}
				else
				{
					// Colliding from below
					data.playerPos.y = blockRect.y + blockRect.w;
					data.velocity.y = 0;
				}
			}
		}
	}
#pragma endregion

	bunnyLogic(deltaTime, gravity, data.playerPos, jumpVelocity, blocks);

#pragma region Mouse Interaction
	// Get the current mouse position relative to the window (screen space)
	glm::vec2 screenMouse = platform::getRelMousePosition();

	// Convert the screen space mouse position to world space
	// This accounts for the camera's current position (scroll offset)
	glm::vec2 worldMouse = glm::vec2(screenMouse) + renderer.currentCamera.position;

	// Snap the mouse position to the nearest 50x50 grid cell
	glm::vec2 snappedMouse = glm::floor(worldMouse / 50.0f) * 50.0f;


	InventorySlot& slot = inventory[selectedSlot];
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
		if (!exists && slot.occupied && !slot.isTool && slot.count > 0)
		{
			// Check selected slot for block to place
			InventorySlot& slot = inventory[selectedSlot];

			if (slot.occupied && slot.count > 0)
			{
				blocks.emplace_back(snappedMouse, static_cast<BlockType>(slot.itemID));
				slot.count--;

				if (slot.count <= 0)
				{
					slot.occupied = false;
					slot.itemID = -1;
				}
			}
		}
	}

	// Destroy the block at the snapped position if right mouse is pressed
	if (platform::isRMousePressed())
	{
		for (int i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i].position == snappedMouse)
			{
				BlockType brokenType = blocks[i].type;

				// Require pickaxe to mine stone
				InventorySlot& heldSlot = inventory[selectedSlot];
				if (brokenType == BlockType::Stone) {
					if (!(heldSlot.occupied && heldSlot.isTool && heldSlot.itemID == static_cast<int>(ToolType::Pickaxe))) {
						// Can't break stone without a pickaxe
						break;
					}
				}


				// Remove block
				blocks.erase(blocks.begin() + i);

				// Try to stack the item if it already exists
				bool added = false;
				for (auto& slot : inventory)
				{
					if (slot.occupied && slot.itemID == static_cast<int>(brokenType))
					{
						slot.count++; // Stack one more
						added = true;
						break;
					}
				}

				// If not stacked, try to add to a new slot
				if (!added)
				{
					for (auto& slot : inventory)
					{
						if (!slot.occupied)
						{
							slot.occupied = true;
							slot.itemID = static_cast<int>(brokenType);
							slot.count = 1;
							break;
						}
					}
				}
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


		if (inventory[index].occupied)
		{


			// Choose the right texture instead of color
			gl2d::Texture* blockTex = nullptr;
			
			//glm::vec4 color = inventory[index].isTool ? Colors_Orange : Colors_White;
			switch (static_cast<BlockType>(inventory[index].itemID))
			{
			case BlockType::Grass: 
				blockTex = &dirtTexture; 
				break;
			case BlockType::Stone: 
				blockTex = &cobblestroneTexture; 
				break;
			case BlockType::Water: 
				blockTex = &tempTexture; 
				break;
			default: 
				break;
			}

			gl2d::Texture* toolTex = nullptr;
			switch (static_cast<ToolType>(inventory[index].itemID))
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

			// Render texture in inventory slot
			if (blockTex)
			{
				renderer.renderRectangle(
					{ pos + glm::vec2(8, 8), slotSize - 16, slotSize - 16 },
					*blockTex
				);
			}
			else if (toolTex)
			{
				renderer.renderRectangle(
					{ pos + glm::vec2(8, 8), slotSize - 16, slotSize - 16 },
					* toolTex
				);
			}


			if (inventory[index].count > 1)
			{
				char countText[8];
				snprintf(countText, sizeof(countText), "%d", inventory[index].count);

				renderer.renderText(
					pos + glm::vec2(4, 4),
					countText,
					font,
					Colors_White,
					0.6f,
					0.0f,
					0.0f
				);
			}
		}
	}

	// ---- Draw remaining inventory if open ----
	if (inventoryOpen)
	{
		for (int row = 0; row < inventoryRows; ++row)
		{
			for (int col = 0; col < inventoryCols; ++col)
			{
				int index = (persistentTopRows + row) * inventoryCols + col;
				glm::vec2 pos = startPos + glm::vec2(col * (slotSize + slotPadding), (row + 1) * (slotSize + slotPadding));

				renderer.renderRectangle({ pos, slotSize, slotSize }, Colors_Gray);

				if (inventory[index].occupied)
				{
					// Choose the right texture instead of color
					gl2d::Texture* blockTex = nullptr;

					//glm::vec4 color = inventory[index].isTool ? Colors_Orange : Colors_White;
					switch (static_cast<BlockType>(inventory[index].itemID))
					{
					case BlockType::Grass:
						blockTex = &dirtTexture;
						break;
					case BlockType::Stone:
						blockTex = &cobblestroneTexture;
						break;
					case BlockType::Water:
						blockTex = &tempTexture;
						break;
					default:
						break;
					}

					gl2d::Texture* toolTex = nullptr;
					switch (static_cast<ToolType>(inventory[index].itemID))
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

					// Render texture in inventory slot
					if (blockTex)
					{
						renderer.renderRectangle(
							{ pos + glm::vec2(8, 8), slotSize - 16, slotSize - 16 },
							*blockTex
						);
					}
					else if (toolTex)
					{
						renderer.renderRectangle(
							{ pos + glm::vec2(8, 8), slotSize - 16, slotSize - 16 },
							*toolTex
						);
					}

					if (inventory[index].count > 1)
					{
						char countText[8];
						snprintf(countText, sizeof(countText), "%d", inventory[index].count);

						renderer.renderText(
							pos + glm::vec2(4, 4),
							countText,
							font,
							Colors_White,
							0.6f,
							0.0f,
							0.0f
						);
					}
				}
			}
		}
	}

#pragma endregion	



	renderer.flush();
#pragma endregion

	return true;
}

void closeGame()
{
	// Cleanup if needed
}
