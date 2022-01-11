#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <chrono>
#include <fstream>
#include "Chunk.h"
#include <robin_hood.h>

class Entity;

class World
{
	std::string m_filename;

	std::ifstream m_inFile;

	// keep track of last loading time for playtime accumulation
	std::chrono::steady_clock::time_point m_timeSinceLoad;

	// File members to save/load
	std::string m_version;
	std::string m_worldname;
	long long m_seed;
	long long m_timeCreated; // in seconds
	long long m_lastPlayed; // in seconds
	long long m_timePlayed; // in seconds
	robin_hood::unordered_map<size_t, Entity*> m_entities;


	// split world into chunks

	// terrain voxels will be split up into sections

	// everything related to this game is almost
	// all 3d

	// packet/rpc abstractions have already been handled
	// graphics engine and physics crap remains

	//
	

public:
	World(std::string name);

	//void GenerateHeader(std::string version,
	//	std::string worldname,
	//	long long seed = 0);

	void Save();

	void LoadHeaderSection(std::string defVersion, std::string defWorldname, long long defSeed = 0);
	void LoadEntitySection();
	
};

#endif
