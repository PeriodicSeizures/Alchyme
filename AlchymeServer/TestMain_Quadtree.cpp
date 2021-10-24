#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Quadtree.h"
#include <chrono>

struct Entity {
	float x, y;
};

std::unordered_map<int, Entity> entities;
//std::vector<Entity> entities(1000000);

static void my_callback(int id, float& x, float& y) {
	auto&& entity = entities[id];
	x = entity.x, y = entity.y;
}

int main() {
	// Randomnize seed
	//srand(time(NULL));

	std::vector<int> retrieved;
	Quadtree<float, my_callback> my_quadtree(10.f, 10.f, 2000.f, 2000.f);

	int trials;
	std::cout << "Input trials: ";
	std::cin >> trials;
	std::cout << std::endl;

	std::cout << "Inserting entities..." << std::endl;

	auto BEGIN_TIME = std::chrono::steady_clock::now();
	for (int i = 0; i < trials; i++) {
		float x = 11 + (rand() % 1988);
		float y = 11 + (rand() % 1988);

		//entities.insert({ i, Entity(x, y) });
		entities[i] = Entity(x, y);
		if (!my_quadtree.insert(i))
			std::cerr << "Failed to insert: " << i << " (" << x << ", " << y << ")" << std::endl;

		//std::cout << "id: " << i << ", (" << x << ", " << y << ")" << std::endl;
	}

	// Benchmark time to insert
	auto NOW = std::chrono::steady_clock::now();
	auto TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - BEGIN_TIME).count() / 1000.f;
	std::cout << "Inserted " << my_quadtree.size() << "/" << trials << " entities over " << TIME_IT_TOOK << "s" << std::endl;
	std::cout << "- - - - -" << std::endl;



	// Find
	std::cout << "Finding objects..." << std::endl;
	BEGIN_TIME = std::chrono::steady_clock::now();
	//my_quadtree.retrieve(0, 0, 1000, 1000, retrieved);
	my_quadtree.retrieve(500, 500, 500, retrieved);

	// Benchmark time to retrieve
	NOW = std::chrono::steady_clock::now();
	TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - BEGIN_TIME).count() / 1000.f;

	std::cout << "Found " << retrieved.size() << " objects in: " << TIME_IT_TOOK << "s" << std::endl;
	std::cout << "- - - - -" << std::endl;



	// Remove those nodes
	//BEGIN_TIME = std::chrono::steady_clock::now();
	for (int node : retrieved) {
		//my_quadtree.
		my_quadtree.remove(node);
	}

	my_quadtree.render();

	return 0;
}