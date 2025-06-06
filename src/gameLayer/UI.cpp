#include "UI.h"
#include "platformInput.h"

// State
std::vector<InventorySlot> inventory(inventoryCols* totalInventoryRows);
std::vector<DroppedItem> droppedItems;
int selectedSlot = 0;
bool inventoryOpen = false;

void startItemsInventory()
{
    inventory[0] = { true, static_cast<int>(ToolType::Pickaxe), 1, true };
    inventory[1] = { true, static_cast<int>(ToolType::Axe), 1, true };
    inventory[2] = { true, static_cast<int>(ToolType::Sword), 1, true };
	inventory[3] = { true, static_cast<int>(BlockType::Chest), 1, false };
}

void handleInventoryInput()
{
    // Toggle inventory
    if (platform::isButtonPressedOn(platform::Button::Tab)) {
        inventoryOpen = !inventoryOpen;
    }

    // Number keys for top bar
    for (int i = 0; i < 9; ++i) {
        if (platform::isButtonPressedOn(platform::Button::NR1 + i)) {
            selectedSlot = i;
            break;
        }
    }
    if (platform::isButtonPressedOn(platform::Button::NR0)) {
        selectedSlot = 9;
    }
}

InventorySlot& getSelectedSlot()
{
    return inventory[selectedSlot];
}


void inventoryOccupied(gl2d::Renderer2D& renderer, int index, gl2d::Font font, glm::vec2 pos)
{
	if (inventory[index].occupied)
	{
		// Choose the right texture instead of color
		gl2d::Texture* blockTex = nullptr;

		//glm::vec4 color = inventory[index].isTool ? Colors_Orange : Colors_White;
		switch (static_cast<BlockType>(inventory[index].itemID))
		{
		case BlockType::Grass: blockTex = &dirtTexture; break;
		case BlockType::Stone: blockTex = &cobblestroneTexture; break;
		case BlockType::Wood:  blockTex = &woodTexture; break;
		case BlockType::Water: blockTex = &tempTexture; break;
		case BlockType::Chest: blockTex = &chestTexture; break;
		default: break;
		}

		gl2d::Texture* toolTex = nullptr;
		switch (static_cast<ToolType>(inventory[index].itemID))
		{
		case ToolType::Pickaxe: toolTex = &pickaxeTexture; break;
		case ToolType::Axe:     toolTex = &axeTexture; break;
		case ToolType::Sword:   toolTex = &swordTexture; break;
		default: break;
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

