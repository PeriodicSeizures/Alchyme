#ifndef WORLD_H
#define WORLD_H

#include <string>

class World
{
	std::string m_filename;
	std::string m_worldname;
	
	size_t m_seed;
	std::string m_version;

	// split world into chunks

	// terrain voxels will be split up into sections

	// everything related to this game is almost
	// all 3d

	// packet/rpc abstractions have already been handled
	// graphics engine and physics crap remains

	//
	

public:
	World(std::string name);

	void GenerateHeader(std::string worldname,
		std::string version, 
		size_t seed = 0);

	void Save();

	void Load();
	
};

#endif
