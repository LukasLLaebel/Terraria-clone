# 🌲 Terraria Clone
A simple 2D game inspired by Terraria, currently being developed in C++.

## 🎮 Features
- 👤 Player movement
- 🧱 Placeable and destructible block platforms
- 🔨 Build and destroy mechanics
	- 👁️ LOS (Line Of Sigt) added
- 🎥 Camera that follows the player
- Collision optimised in utilities
- 🧱 3 types of blocks
	- 🟫 Dirt
	- 🏔️ Stone
	- 🌊 Water
- 🗺️ World genration
	- 🌀 Added Perlin noise (smoother world gen)
	- 🌳 Added trees to world generation
	- 🕳️ Trees are walk throw have no collistion
	- ⚒ Only axe can destroy trees
	- 💥 Destroy one block and trees fell
	- 2DPerlin noise and fractalPerlin noise added for cave generation
- 🎒 Player inventory
	- 🔄 Choose between items
	- 📦 Store blocks
- 🖼️ Textures
	- 👕 Player texture
	- 🧰 Tool texture
		- 🗡️ Sword
		- ⛏️ Pickaxe
		- ⚒ Axe
- ⚒️ Tool functionalities
	- ⛏️ pickaxe 
- 🐾 Enteties
	- 🐰 Bunnies
	- 🦔 Hedgehogs

## 🚀 Setup
This project uses meemknight's excellent C++ game development setup.
His tutorials and resources were extremely helpful in getting started — highly recommended!
[Watch is channel](https://www.youtube.com/@lowlevelgamedev9330)

### 📁 Project Base
- 🔧 Setup Files [SpaceGame - Full Course](https://github.com/meemknight/game-in-cpp-full-course/tree/6f51a211a626f1af1988946a25c162a612fa1f57)

- 🧠 Full Source Code [SpaceGame - Full Game](https://github.com/meemknight/game-in-cpp-full-course)

## 📝 Todo
- background
- Clean up world genration (fix water generation)
- Clean up world genration (different types of mountains)
- Water works
- Structures
	- dungeon
	- sandtemple
	- Beehive
	- Minesharfts
	- improve caves
- Enteties
	- Bosses
	- Zomibes
	- Eyes
	- Slimes
	- Vultures
	- Sharks
	- Garden Gnomes
- fix bug. "when player gets out of the map numbers of items float to the left"
- fix bug. "can go throw blocks if on same axis"
- fix bug. "fix the collision jitter"
- fix bug. "entities can jump up against trees??"
- move items arount in inventory
- Health
- Mana
- Tools
	- Hammer
- NPCs
	- Dialog
	- Buy items
- Chests
- Doors
- More blocks
- block background
- Crafting
- Textures
- be able to safe game (safesave)
- Multiplayer
- Animations
- Improve trees
	- Trees drop planks depending on height and as items
- Shoud be able to drop items
- Player can organise items in inventory
- Optimise for lagging
- Armor
- Coins
- Shaders

## 📌 Notes
This is a work in progress, and I'm actively developing new features.
Feel free to watch the repo or contribute ideas!
