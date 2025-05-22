#include "blocks.h"
#include <gl2d/gl2d.h>






Block::Block(glm::vec2 pos, BlockType t)
    : position(pos), type(t)
{
}

void Block::render(gl2d::Renderer2D &renderer) const
{
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
    case BlockType::Custom: return "Custom";
    default: return "Unknown";
    }
}
