#include <algorithm>
#include <gl2d/gl2d.h>
#include <vector>
#include <blocks.h>

using std::min;
using std::max;


struct Block;
struct GameplayData;

extern std::vector<Block> blocks;

void Collision(glm::vec2& entityPos, bool& isOnGround, glm::vec2 entitySize, glm::vec2& velocity);
