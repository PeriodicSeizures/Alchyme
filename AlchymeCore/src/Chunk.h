#pragma once

class World;

class Chunk {

	static constexpr int SIZE = 32;
	#define BLOCK_ID_MASK	0xFFF00000ull
	#define BLOCK_DATA_MASK 0x000FF

	int x, y;

	unsigned long long blocks[SIZE * SIZE];

public:
	Chunk(int x, int y);

	static void SetWorld(World* w);
};
