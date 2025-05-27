#include <utilities.h>


// collision
void Collision(glm::vec2& entityPos, bool& isOnGround, glm::vec2 entitySize, glm::vec2& velocity)
{
	isOnGround = false;

	glm::vec4 entityRect = { entityPos.x, entityPos.y, entitySize.x, entitySize.y };

	for (auto& block : blocks)
	{

		// Skip collision for Wood and Leaves blocks
		if (block.type == BlockType::Wood || block.type == BlockType::Leaves)
		{
			continue;
		}

		glm::vec4 blockRect = { block.position.x, block.position.y, block.size.x, block.size.y };

		bool xOverlap = entityRect.x < blockRect.x + blockRect.z &&
			entityRect.x + entityRect.z > blockRect.x;
		bool yOverlap = entityRect.y < blockRect.y + blockRect.w &&
			entityRect.y + entityRect.w > blockRect.y;

		if (xOverlap && yOverlap)
		{
			// Calculate penetration depths
			float fromLeft = (entityRect.x + entityRect.z) - blockRect.x;
			float fromRight = (blockRect.x + blockRect.z) - entityRect.x;
			float fromTop = (entityRect.y + entityRect.w) - blockRect.y;
			float fromBottom = (blockRect.y + blockRect.w) - entityRect.y;

			float minHoriz = min(fromLeft, fromRight);
			float minVert = min(fromTop, fromBottom);

			if (minHoriz < minVert)
			{
				// Horizontal collision
				if (fromLeft < fromRight)
				{
					// Colliding from left
					entityPos.x = blockRect.x - entitySize.x;
				}
				else
				{
					// Colliding from right
					entityPos.x = blockRect.x + blockRect.z;
				}
				velocity.x = 0;
			}
			else
			{
				// Vertical collision
				if (fromTop < fromBottom)
				{
					// Colliding from above (landing)
					entityPos.y = blockRect.y - entitySize.y;
					velocity.y = 0;
					isOnGround = true;
				}
				else
				{
					// Colliding from below
					entityPos.y = blockRect.y + blockRect.w;
					velocity.y = 0;
				}
			}
		}
	}
}