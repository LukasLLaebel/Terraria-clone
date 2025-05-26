#include <entities.h>

std::vector<Bunny> bunnies;
float bunnySpawnTimer = 10.0f;

#pragma region Bunny
void bunnySpawn(float deltaTime, glm::vec2 playerPos)
{
	bunnySpawnTimer -= deltaTime;
	if (bunnySpawnTimer <= 0.0f)
	{
		bunnySpawnTimer = 5.0f + rand() % 5; // Spawn interval 5–10s

		glm::vec2 spawnPos = playerPos + glm::vec2((rand() % 200 - 100), -50); // Random nearby x offset
		bunnies.push_back({ spawnPos, {0, 0}, 0.0f });
	}
}

void bunnyLogic(float deltaTime, float gravity, glm::vec2 playerPos, float jumpVelocity, std::vector<Block>& blocks)
{
	for (auto& bunny : bunnies)
	{
		bunny.velocity.y += (gravity * 2) * deltaTime;

		// Distance to player
		glm::vec2 toPlayer = playerPos - bunny.position;
		float distance = glm::length(toPlayer);

		bunny.jumpCooldown -= deltaTime;
		if (bunny.jumpCooldown <= 0.0f && bunny.onGround)
		{
			bunny.jumpCooldown = bunny.jumpInterval;
			bool facingRight = false;
			// Determine jump direction
			if (distance < 200.0f)
			{
				bunny.velocity.x = (bunny.position.x > playerPos.x) ? 100.0f : -100.0f;
			}
			else
			{
				bunny.velocity.x = (rand() % 2 == 0) ? 100.0f : -100.0f;
			}

			// Set facing direction based on velocity
			if (bunny.velocity.x > 0)
			{
				bunny.facing = FacingDirection::Right;
			}
			else if (bunny.velocity.x < 0)
			{
				bunny.facing = FacingDirection::Left;
			}


			// Apply jump
			bunny.velocity.y = jumpVelocity;
			bunny.onGround = false;
		}

		// Move bunny
		bunny.position += bunny.velocity * deltaTime;

		// Simple ground collision
		bunny.onGround = false;
		glm::vec4 bunnyRect = { bunny.position.x, bunny.position.y, 40, 40 };

		for (auto& block : blocks)
		{
			glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

			bool isTouching =
				bunnyRect.x < blockRect.x + blockRect.z &&
				bunnyRect.x + bunnyRect.z > blockRect.x &&
				bunnyRect.y + bunnyRect.w > blockRect.y &&
				bunnyRect.y + bunnyRect.w < blockRect.y + blockRect.w;

			if (isTouching)
			{
				bunny.position.y = block.position.y - 40;
				bunny.velocity.y = 0;
				bunny.onGround = true;
				bunny.velocity.x = 0; // Stop horizontal movement after landing
				break;
			}

		}
	}
}
#pragma endregion









