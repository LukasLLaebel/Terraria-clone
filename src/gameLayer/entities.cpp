#include <entities.h>
#include <utilities.h>

std::vector<Bunny> bunnies;
float bunnySpawnTimer = 10.0f;

std::vector<Hedgehog> hedgehogs;
float hedgehogSpawnTimer = 10.0f;


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

#pragma region Hedgehogs

void hedgehogSpawn(float deltaTime, glm::vec2 playerPos)
{
	hedgehogSpawnTimer -= deltaTime;
	if (hedgehogSpawnTimer <= 0.0f)
	{
		hedgehogSpawnTimer = 5.0f + rand() % 5; // Spawn interval 5–10s

		glm::vec2 spawnPos = playerPos + glm::vec2((rand() % 200 - 100), -50); // Random nearby x offset
        hedgehogs.push_back({
            spawnPos,
            {0, 0},
            0.0f,           // Start with no cooldown
            1.0f,           // Wait 1 second before jumping over a block
            FacingDirection::Right,
            true
        });
	}
}
void hedgehogLogic(float deltaTime, float gravity, glm::vec2 playerPos, float jumpVelocity, std::vector<Block>& blocks)
{
	const float moveSpeed = 20.0f;

	for (auto& hedgehog : hedgehogs)
	{
		// Apply gravity
		hedgehog.velocity.y += gravity * deltaTime;

		// Move horizontally
		hedgehog.velocity.x = (hedgehog.facing == FacingDirection::Right) ? moveSpeed : -moveSpeed;

		// Update position
		hedgehog.position += hedgehog.velocity * deltaTime;

		// Use universal collision method to adjust position and set onGround
		Collision(hedgehog.position, hedgehog.onGround, { 40, 40 }, hedgehog.velocity);

		// Probe in front to check if a block is blocking the path
		glm::vec2 probeOffset = (hedgehog.facing == FacingDirection::Right) ? glm::vec2(40, 0) : glm::vec2(-5, 0);
		glm::vec2 probePoint = hedgehog.position + probeOffset;
		glm::vec4 probeRect = { probePoint.x, probePoint.y, 5, 40 };

		bool wallAhead = false;

		for (auto& block : blocks)
		{
			glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

			bool xOverlap = probeRect.x < blockRect.x + blockRect.z &&
				probeRect.x + probeRect.z > blockRect.x;
			bool yOverlap = probeRect.y < blockRect.y + blockRect.w &&
				probeRect.y + probeRect.w > blockRect.y;

			if (xOverlap && yOverlap)
			{
				wallAhead = true;
				break;
			}
		}

		// Jump if there's a block in front and the hedgehog is on ground
		if (wallAhead && hedgehog.onGround && hedgehog.jumpCooldown <= 0.0f)
		{
			hedgehog.velocity.y = jumpVelocity;
			hedgehog.onGround = false;
			hedgehog.jumpCooldown = hedgehog.jumpInterval;
		}

		// Cliff detection – if no ground ahead, turn around
		glm::vec2 footProbe = hedgehog.position + glm::vec2((hedgehog.facing == FacingDirection::Right ? 25 : -5), 45);
		bool groundAhead = false;

		for (auto& block : blocks)
		{
			glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

			if (footProbe.x >= blockRect.x && footProbe.x <= blockRect.x + blockRect.z &&
				footProbe.y >= blockRect.y && footProbe.y <= blockRect.y + blockRect.w)
			{
				groundAhead = true;
				break;
			}
		}

		if (!groundAhead && hedgehog.onGround)
		{
			// Turn around if a cliff is ahead
			hedgehog.facing = (hedgehog.facing == FacingDirection::Right) ? FacingDirection::Left : FacingDirection::Right;
		}

		// Decrease jump cooldown
		hedgehog.jumpCooldown -= deltaTime;
	}
}

#pragma endregion








