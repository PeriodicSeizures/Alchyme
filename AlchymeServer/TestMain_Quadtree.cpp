#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Quadtree.h"
#include <chrono>

struct Entity {
	float x, y;
};

static std::unordered_map<int, Entity> entities;
//static std::vector<Entity> entities;

static void my_callback(int id, float& x, float& y) {
	auto&& entity = entities[id];
	x = entity.x, y = entity.y;
}

bool anyOverlap(int xMin,
				int yMin,
				int xMax,
				int yMax,

				int m_xMin,
				int m_yMin,
				int m_xMax,
				int m_yMax
	) {

	// Return whether THAT RECT and THIS RECT overlap

	// WHETHER
	return m_xMin < xMax &&
		m_xMax > xMin &&
		m_yMin < yMax &&
		m_yMax > yMin;
}


int main() {

	//render();
	//return 0;

	// WILL OVERLAP
	//std::cout << "Overlap: " << 
	//	anyOverlap(219, 95, 219+200, 95 + 100,
	//		358, -29, 358 + 270, -29 + 150) << "\n";

	// NO OVERLAP
	//std::cout << "Overlap: " <<
	//	anyOverlap(201, 142, 201 + 200, 142 + 100,
	//		358, -29, 358 + 270, -29 + 150) << "\n";

	// OVERLAP
	//std::cout << "Overlap: " <<
	//	anyOverlap(396, -8, 396 + 200, -8 + 100,
	//		358, -29, 358 + 270, -29 + 150) << "\n";

	// OVERLAP (TEST SWAPPED
	//std::cout << "Overlap: " <<
	//	anyOverlap(358, -29, 358 + 270, -29 + 150,
	//		396, -8, 396 + 200, -8 + 100) << "\n";

	//return 0;






	Quadtree<float, my_callback> my_quadtree(10.f, 10.f, 2000.f, 2000.f);

	int trials;
	std::cout << "Input trials: ";
	std::cin >> trials;

	srand(time(NULL));

	std::cout << "\nInserting entities...\n";


	auto BEGIN_TIME = std::chrono::steady_clock::now();
	for (int i = 0; i < trials; i++) {
		int id = i; // rand();
		//float x = 11 + (rand() % 1988);
		//float y = 11 + (rand() % 1988);

		float x = 11 + (rand() % 2000);
		float y = 11 + (rand() % 12000);

		entities.insert({ id, Entity(x, y) });
		if (!my_quadtree.insert(id)) {
			std::cout << "Failed to insert: " << id << ", (" << x << ", " << y << ")\n";
		}

		//std::cout << "id: " << id << ", (" << x << ", " << y << ")\n";
	}

	auto NOW = std::chrono::steady_clock::now();
	auto TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::microseconds>(NOW - BEGIN_TIME).count();
	std::cout << "Inserted " << my_quadtree.size() << "/" << trials << " entities over " << ((float)TIME_IT_TOOK/1000000.f) << 
		"s\n- - - - -\n";

	std::vector<int> ids;

	std::cout << "Finding objects...\n";

	BEGIN_TIME = std::chrono::steady_clock::now();
	my_quadtree.retrieve(0, 0, 1000, 1000, ids);
	//my_quadtree.retrieve(250, 250, 450, 450, ids);
	//my_quadtree.retrieve(-1001, -1001, 1001, 1001, ids);
	//my_quadtree.retrieve(0, 0, 2001, 2001, ids);

	NOW = std::chrono::steady_clock::now();
	TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::microseconds>(NOW - BEGIN_TIME).count();

	std::cout << "Found " << ids.size() << " objects in: " << ((float)TIME_IT_TOOK / 1000000.f) << "s\n";
	//for (auto id : ids) {
	//	std::cout << id << ", ";
	//}

	std::cout << "\n";

	my_quadtree.render();

	return 0;
}