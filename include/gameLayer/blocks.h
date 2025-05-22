#pragma once
#include <glm/glm.hpp>
#include <string>
#include <gl2d/gl2d.h>

//extern gl2d::Texture grassTexture;
extern gl2d::Texture dirtTexture;
extern gl2d::Texture cobblestroneTexture;
extern gl2d::Texture tempTexture;

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