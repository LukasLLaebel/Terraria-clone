#pragma once
#include <gl2d/gl2d.h>
//#include <glm/glm.hpp>
#include <vector>
#include <blocks.h>

enum class FacingDirection { Left, Right };

struct Bunny
{
	glm::vec2 position;
	glm::vec2 velocity;
	float jumpCooldown = 0.0f;
	float jumpInterval = 1.0f;
	FacingDirection facing = FacingDirection::Left;
	bool onGround = false;
	const glm::vec2 Size = { 40, 40 };
};

struct Block;
struct GameplayData;

void bunnyLogic(float deltaTime, float gravity, glm::vec2 playerPos, float JumpVelocity, std::vector<Block>& blocks);
void bunnySpawn(float deltaTime, glm::vec2 playerPos);


extern std::vector<Bunny> bunnies;
extern float bunnySpawnTimer;
