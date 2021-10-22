#pragma once
#include <string>

class World
{
	std::string m_name;
	size_t m_seed;

	// split world into chunks

	// terrain voxels will be split up into sections

	// everything related to this game is almost
	// all 3d

	// packet/rpc abstractions have already been handled
	// graphics engine and physics crap remains

	// 
	void Save();

	void Load();

public:
	World(std::string name);
	World(std::string name, size_t seed);
	
};

