#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Quadtree.h"
#include <chrono>

struct Entity {
	float x, y;
};

//std::unordered_map<int, Entity> entities;
std::vector<Entity> entities(10000);

static void my_callback(int id, float& x, float& y) {
	auto&& entity = entities[id];
	x = entity.x, y = entity.y;
}

int main() {
	std::vector<int> retrieved;
	Quadtree<float, my_callback> my_quadtree(10.f, 10.f, 2000.f, 2000.f);



	std::cout << "Inserting entities..." << std::endl;
	auto BEGIN_TIME = std::chrono::steady_clock::now();
	for (int i = 0; i < entities.size(); i++) {
		float x = 11 + (rand() % 1988);
		float y = 11 + (rand() % 1988);

		entities[i] = Entity(x, y);
		my_quadtree.insert(i);
		//std::cout << "id: " << i << ", (" << x << ", " << y << ")" << std::endl;
	}



	// Benchmark time to insert
	auto NOW = std::chrono::steady_clock::now();
	auto TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - BEGIN_TIME).count() / 1000.f;
	std::cout << "Inserted " << my_quadtree.size() << "/" << entities.size() << " entities over " << TIME_IT_TOOK << "s" << std::endl;
	std::cout << "- - - - -" << std::endl;

	float x1 = 1001, y1 = 1001, x2 = 1450, y2 = 1780;

	for (int i = 0; i < 5000000; i++) {
		auto BEGIN_TIME = std::chrono::steady_clock::now();
		//for (int node = 0; node < entities.size(); node++) {
		//	// get all entities within this entity
		//	auto&& e = entities[node];
		//	my_quadtree.retrieve(e.x, e.y, 100, retrieved);
		//}

		my_quadtree.retrieve(x1, y1, x2, y2, retrieved);

		auto NOW = std::chrono::steady_clock::now();
		auto TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - BEGIN_TIME).count() / 1000.f;
		//std::cout << "Took " << TIME_IT_TOOK << "s (trial " << i << ")" << std::endl;
	}

	std::cout << "Retrieved cumulative: " << retrieved.size() << "\n";

	//my_quadtree.render();

	return 0;
}