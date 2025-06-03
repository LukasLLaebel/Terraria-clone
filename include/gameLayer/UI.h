#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <gl2d/gl2d.h>
#include <blocks.h>

enum class ToolType {
    None = -1,
    Pickaxe = 100,
    Axe,
    Sword
};


struct InventorySlot {
    bool occupied = false;
    int itemID = -1;
    int count = 0;
    bool isTool = false;
};

struct DroppedItem {
    glm::vec2 position;
    glm::vec2 velocity;
    int itemID;
    float lifetime = 30.0f;
};

// Constants
constexpr int inventoryCols = 10;
constexpr int totalInventoryRows = 5;
constexpr int persistentTopRows = 1;
constexpr int inventoryRows = totalInventoryRows - persistentTopRows;
constexpr int slotSize = 50;
constexpr int slotPadding = 4;

// Extern variables
extern std::vector<InventorySlot> inventory;
extern std::vector<DroppedItem> droppedItems;
extern int selectedSlot;
extern bool inventoryOpen;

// tool textures
extern gl2d::Texture pickaxeTexture;
extern gl2d::Texture axeTexture;
extern gl2d::Texture swordTexture;

// Prototypes
void startItemsInventory();
void handleInventoryInput();
InventorySlot& getSelectedSlot();
void inventoryOccupied(gl2d::Renderer2D& renderer, int index, gl2d::Font font, glm::vec2 pos);

//void persistentInventory(gl2d::Renderer2D& renderer, glm::vec2 startPos, gl2d::Font font);