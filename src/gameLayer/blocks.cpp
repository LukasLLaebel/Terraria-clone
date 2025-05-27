#include "blocks.h"
#include <gl2d/gl2d.h>






Block::Block(glm::vec2 pos, BlockType t)
    : position(pos), type(t)
{
}

void Block::render(gl2d::Renderer2D &renderer) const
{
    // leaves is a bigger block so we make another rectangle for it where we center it depenend to its size
    glm::vec4 leavesRect = { position.x - size.x * 2, position.y - size.y * 4, size.x * 5, size.y * 5 };
    glm::vec4 rect = { position.x, position.y, size.x, size.y };
    switch (type)
    {
    case BlockType::Grass:
        renderer.renderRectangle(rect, dirtTexture);
        break;
    case BlockType::Stone:
        renderer.renderRectangle(rect, cobblestroneTexture);
        break;
    case BlockType::Water:
        renderer.renderRectangle(rect, tempTexture);
        break;
    case BlockType::Wood:
        renderer.renderRectangle(rect, woodTexture);
        break;
    case BlockType::Leaves:
        renderer.renderRectangle(leavesRect, LeavesTexture);
        break;
    case BlockType::Custom:
    default:
        renderer.renderRectangle(rect, tempTexture);
        break;
    }
}

std::string Block::getName() const
{
    switch (type)
    {
    case BlockType::Grass: return "Grass";
    case BlockType::Stone: return "Stone";
    case BlockType::Water: return "Water";
    case BlockType::Wood: return "Wood";
    case BlockType::Leaves: return "Leaves";
    case BlockType::Custom: return "Custom";
    default: return "Unknown";
    }
}
