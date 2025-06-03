#include <utilities.h>
#include "platformInput.h"

// collision
void Collision(glm::vec2& entityPos, bool& isOnGround, glm::vec2 entitySize, glm::vec2& velocity)
{
	isOnGround = false;

	glm::vec4 entityRect = { entityPos.x, entityPos.y, entitySize.x, entitySize.y };

	for (auto& block : blocks)
	{

		// Skip collision for Wood and Leaves blocks
		if (block.type == BlockType::Wood || block.type == BlockType::Leaves)
		{
			continue;
		}

		glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

		bool xOverlap = entityRect.x < blockRect.x + blockRect.z &&
			entityRect.x + entityRect.z > blockRect.x;
		bool yOverlap = entityRect.y < blockRect.y + blockRect.w &&
			entityRect.y + entityRect.w > blockRect.y;

		if (xOverlap && yOverlap)
		{
			// Calculate penetration depths
			float fromLeft = (entityRect.x + entityRect.z) - blockRect.x;
			float fromRight = (blockRect.x + blockRect.z) - entityRect.x;
			float fromTop = (entityRect.y + entityRect.w) - blockRect.y;
			float fromBottom = (blockRect.y + blockRect.w) - entityRect.y;

			float minHoriz = min(fromLeft, fromRight);
			float minVert = min(fromTop, fromBottom);

			if (minHoriz < minVert)
			{
				// Horizontal collision
				if (fromLeft < fromRight)
				{
					// Colliding from left
					entityPos.x = blockRect.x - entitySize.x;
				}
				else
				{
					// Colliding from right
					entityPos.x = blockRect.x + blockRect.z;
				}
				velocity.x = 0;
			}
			else
			{
				// Vertical collision
				if (fromTop < fromBottom)
				{
					// Colliding from above (landing)
					entityPos.y = blockRect.y - entitySize.y;
					velocity.y = 0;
					isOnGround = true;
				}
				else
				{
					// Colliding from below
					entityPos.y = blockRect.y + blockRect.w;
					velocity.y = 0;
				}
			}
		}
	}
}


void leftMouse(glm::vec2 snappedMouse, InventorySlot& slot)
{
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
}

void rightMouse(glm::vec2 snappedMouse, glm::vec2 playerPos)
{
	// Destroy the block at the snapped position if right mouse is pressed
	if (platform::isRMousePressed())
	{
		for (int i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i].position == snappedMouse)
			{
				// LOS - Line of Sigt
				bool isVisible = true;

				glm::vec2 playerCenter = playerPos + glm::vec2(25.0f, 50.0f); // Center of player
				glm::vec2 targetBlockCenter = blocks[i].position + glm::vec2(25.0f, 25.0f); // Center of block

				glm::vec2 direction = glm::normalize(targetBlockCenter - playerCenter);
				float distance = glm::distance(targetBlockCenter, playerCenter);
				float step = 5.0f;

				for (float d = 0.0f; d < distance; d += step)
				{
					glm::vec2 checkPos = playerCenter + direction * d;

					for (const auto& otherBlock : blocks)
					{
						if (otherBlock.position == blocks[i].position) continue;

						if (checkPos.x >= otherBlock.position.x && checkPos.x < otherBlock.position.x + 50.0f &&
							checkPos.y >= otherBlock.position.y && checkPos.y < otherBlock.position.y + 50.0f)
						{
							isVisible = false;
							break;
						}
					}

					if (!isVisible)
						break;
				}

				if (!isVisible)
					break; // Exit early — block is not in line of sight




				BlockType brokenType = blocks[i].type;
				InventorySlot& heldSlot = inventory[selectedSlot];

				// Check for correct tool requirements
				if (brokenType == BlockType::Stone &&
					!(heldSlot.occupied && heldSlot.isTool && heldSlot.itemID == static_cast<int>(ToolType::Pickaxe)))
				{
					break; // Needs pickaxe
				}

				if (brokenType == BlockType::Wood &&
					!(heldSlot.occupied && heldSlot.isTool && heldSlot.itemID == static_cast<int>(ToolType::Axe)))
				{
					break; // Needs axe
				}

				if (brokenType == BlockType::Leaves)
				{
					break; // Don't allow breaking leaves directly
				}

				// Tree felling logic (wood block was broken)
				if (brokenType == BlockType::Wood)
				{
					glm::vec2 currentPos = blocks[i].position;

					while (true)
					{
						bool found = false;

						for (int j = 0; j < blocks.size(); ++j)
						{
							if (blocks[j].position == currentPos &&
								(blocks[j].type == BlockType::Wood || blocks[j].type == BlockType::Leaves))
							{
								BlockType typeToAdd = blocks[j].type;

								// Remove the block
								blocks.erase(blocks.begin() + j);

								// Try to stack in inventory
								bool added = false;
								for (auto& slot : inventory)
								{
									if (slot.occupied && slot.itemID == static_cast<int>(typeToAdd))
									{
										slot.count++;
										added = true;
										break;
									}
								}
								if (!added)
								{
									for (auto& slot : inventory)
									{
										if (!slot.occupied)
										{
											slot.occupied = true;
											slot.itemID = static_cast<int>(typeToAdd);
											slot.count = 1;
											break;
										}
									}
								}

								found = true;
								break; // Continue with next upward block
							}
						}

						if (!found)
							break;

						currentPos.y -= 50.0f; // Move up
					}
					// because the leaves are not centered we center them in blocks.cpp
					// because of that we need to look for the leaves when cutting down trees
					glm::vec2 originalWoodPos = blocks[i].position;
					// After destroying all wood blocks
					for (int i = 0; i < blocks.size(); )
					{
						if (blocks[i].type == BlockType::Leaves)
						{
							glm::vec2 leafPos = blocks[i].position;
							// Check distance to any destroyed wood block (or the original wood pos)
							if (glm::distance(leafPos, originalWoodPos) < 5)
							{
								blocks.erase(blocks.begin() + i);
								continue; // skip i increment
							}
						}
						++i;
					}
				}
				else
				{
					// Regular block breaking
					BlockType typeToAdd = brokenType;
					blocks.erase(blocks.begin() + i);

					// Try stacking
					bool added = false;
					for (auto& slot : inventory)
					{
						if (slot.occupied && slot.itemID == static_cast<int>(typeToAdd))
						{
							slot.count++;
							added = true;
							break;
						}
					}
					if (!added)
					{
						for (auto& slot : inventory)
						{
							if (!slot.occupied)
							{
								slot.occupied = true;
								slot.itemID = static_cast<int>(typeToAdd);
								slot.count = 1;
								break;
							}
						}
					}
				}

				break; // Only break one block or tree per click
			}
		}
	}
}