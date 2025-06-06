#include "interactives.h"
#include <iostream>

ChestBlock::ChestBlock(const glm::vec2& position)
    : Block(position, BlockType::Chest)  // Assuming you have a Chest enum value
{
    // Initialize chest inventory empty or with some items
    items.reserve(27);  // Example size (like Minecraft chest)
}

void ChestBlock::open()
{
    std::cout << "Chest opened!\n";
}

void ChestBlock::close()
{
    std::cout << "Chest closed!\n";

}

void ChestBlock::onInteract()
{
    open();
}