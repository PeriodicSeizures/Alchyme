#include "World.h"
#include <fstream>
#include "Utils.h"



World::World(std::string name) : m_name(name), m_seed(StrHash(name.c_str())) {}

World::World(std::string name, size_t seed) : m_name(name), m_seed(seed) {}

void World::Save() {
	std::ofstream file;

	file.open(m_name + ".bin", std::ios::binary);
	if (file.is_open()) {
		file << m_name;
		file << m_seed;
		file.close();
	}
}

void World::Load() {
	std::ifstream file;

	file.open(m_name + ".bin", std::ios::binary);
	if (file.is_open()) {
		file >> m_name;
		file >> m_seed;
		file.close();
	}
	else {
		// else, generate things from scratch
	}
}