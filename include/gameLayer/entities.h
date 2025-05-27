#pragma once
#include <gl2d/gl2d.h>
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
struct Hedgehog
{
	glm::vec2 position;
	glm::vec2 velocity;
	float jumpCooldown = 0.0f;
	float jumpInterval;
	FacingDirection facing = FacingDirection::Right;
	bool onGround = false;
	const glm::vec2 Size = { 40, 40 };
};

struct Block;
struct GameplayData;

void bunnyLogic(float deltaTime, float gravity, glm::vec2 playerPos, float JumpVelocity, std::vector<Block>& blocks);
void bunnySpawn(float deltaTime, glm::vec2 playerPos);
void hedgehogSpawn(float deltaTime, glm::vec2 playerPos);
void hedgehogLogic(float deltaTime, float gravity, glm::vec2 playerPos, float jumpVelocity, std::vector<Block>& blocks);



extern std::vector<Bunny> bunnies;
extern float bunnySpawnTimer;
extern std::vector<Hedgehog> hedgehogs;
extern float hegdehogSpawnTimer;
