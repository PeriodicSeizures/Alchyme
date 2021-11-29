#include "World.h"
#include <fstream>
#include "Utils.h"
#include <random>

World::World(std::string filename) : m_filename(filename) {}

void World::GenerateHeader(std::string worldname, std::string version, size_t seed) {
	// Random seed
	this->m_worldname = worldname;

	if (seed = 0) {
		std::random_device rd;

		std::mt19937_64 eng(rd());
		std::uniform_int_distribution<unsigned long long> distr;
		this->m_seed = distr(eng);
	}

	// Generate once-on header data
	this->m_version = version;

	//
}

void World::Save() {
	std::ofstream file;

	file.open(m_filename + ".bin"); // , std::ios::binary);
	if (file.is_open()) {
		
		// First element should always be the version
		// for compatibiltiy across different game versions
		// which try to load a world of a given version (older/newer/...)
		file << m_version << std::endl;
		file << m_worldname << std::endl;
		file << m_seed << std::endl;

		// save all entities

		// players

		// terrain

		// everything

		// game time

		// game states

		// inventories

		// current time

		// difficulty

		file.close();
	}
}

void World::Load() {
	std::ifstream file;

	file.open(m_filename + ".bin"); // , std::ios::binary);
	if (file.is_open()) {
		file >> m_version;
		file >> m_worldname;
		file >> m_seed;
		file.close();
	}
	else {
		// else, generate things from scratch
	}
}