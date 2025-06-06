#pragma once

#include "blocks.h"  
#include <vector>

class ChestBlock : public Block
{
public:
    ChestBlock(const glm::vec2& position);

    // Example: Inventory inside chest
    std::vector<int> items;

    // You can add chest-specific functions here
    void open();
    void close();

    // Override any Block methods if necessary
    void onInteract() override;
};