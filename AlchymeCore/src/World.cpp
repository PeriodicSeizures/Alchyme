#include "World.h"
#include <fstream>
#include "Utils.h"
#include <random>

using namespace std::chrono;

World::World(std::string filename) : m_filename(filename) {}

void World::Save() {

	const auto now = std::chrono::steady_clock::now();

	m_lastPlayed = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count();
	auto diff = duration_cast<seconds>(now - m_timeSinceLoad).count();
	m_timePlayed += diff;

	m_timeSinceLoad = steady_clock::now();

	std::ofstream file;

	file.open(m_filename); // , std::ios::binary);
	if (file.is_open()) {

		/**
		* File sections
		*	- header section
		*	- 
		*/

		// Cross-version consistent header section
		file << m_version << std::endl;
		file << m_worldname << std::endl;
		file << m_seed << std::endl;
		file << m_timeCreated << std::endl;
		file << m_lastPlayed << std::endl;
		file << m_timePlayed << std::endl;
		// maybe a splash
		// 64x64 of full rgb = 12288 bytes = ~12.3 kb
		// m



		// Now everything else will cater based on version
		// blocks for 

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

void World::LoadHeaderSection(std::string defVersion, std::string defWorldname, long long defSeed) {

	m_timeSinceLoad = steady_clock::now();

	m_inFile.open(m_filename); // , std::ios::binary);
	if (m_inFile.is_open()) {
		std::getline(m_inFile, m_version);
		std::getline(m_inFile, m_worldname);
		m_inFile >> m_seed;
		m_inFile >> m_timeCreated;
		m_inFile >> m_lastPlayed;
		m_inFile >> m_timePlayed;
		//m_inFile.close();
	}
	else {
		// else, generate things from scratch
		this->m_version = defVersion;
		this->m_worldname = defWorldname;

		if (defSeed == 0) {
			std::random_device rd;

			std::mt19937_64 eng(rd());
			std::uniform_int_distribution<long long> distr;
			this->m_seed = distr(eng);
		}

		// Generate once-on header data
		m_timeCreated = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count();
		//m_lastPlayed = noassignment
		m_timePlayed = 0;
	}
}

void World::LoadEntitySection() {
	// players

	// entities


}