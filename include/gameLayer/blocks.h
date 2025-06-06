#pragma once
#include <glm/glm.hpp>
#include <string>
#include <gl2d/gl2d.h>

//extern gl2d::Texture grassTexture;
extern gl2d::Texture dirtTexture;
extern gl2d::Texture cobblestroneTexture;
extern gl2d::Texture woodTexture;
extern gl2d::Texture LeavesTexture;
extern gl2d::Texture tempTexture;
extern gl2d::Texture chestTexture;

enum class BlockType
{
    Grass,
    Stone,
    Water,
    Wood,     
    Leaves,    
    Chest,
    Custom

};

class Block
{
public:
    glm::vec2 position;
    glm::vec2 size = { 50.0f, 50.0f };
    BlockType type;

    Block(glm::vec2 pos, BlockType t);
    virtual ~Block() = default;  // Virtual destructor for polymorphism

    virtual void render(gl2d::Renderer2D& renderer) const;
    virtual std::string getName() const;

    // Virtual method for interaction (can be overridden)
    virtual void onInteract();
};