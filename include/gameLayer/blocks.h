#pragma once
#include <glm/glm.hpp>
#include <string>
#include <gl2d/gl2d.h>

enum class BlockType
{
    Grass,
    Stone,
    Water,
    Custom
};

struct Block
{
    glm::vec2 position;
    glm::vec2 size = { 50.0f, 50.0f };
    BlockType type;

    Block(glm::vec2 pos, BlockType t);
    void render(gl2d::Renderer2D& renderer) const;
    std::string getName() const;
};