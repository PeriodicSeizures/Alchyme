#include <unordered_map>
#include <iostream>
#include "Quadtree.h"

struct Entity {
	float x, y;
};

static std::unordered_map<int, Entity> entity_map;

static void my_callback(int id, float& x, float& y) {
	auto&& entity = entity_map[id];
	x = entity.x, y = entity.y;
}

int main() {
	Quadtree<float, my_callback> my_quadtree(-1000.f, -1000.f, 1000.f, 1000.f);

	//for (int i = 0; i < rand() % 10 + 10; i++) {
	//	int id = rand() % 89999 + 10000;
	//	entity_map.insert({ id, Entity() });
	//	my_quadtree.insert(id);
	//}

	entity_map.insert({ 9, Entity(0, 0) });
	entity_map.insert({ 54, Entity(100, 200) });
	entity_map.insert({ 46, Entity(300, -300) });
	entity_map.insert({ 15, Entity(-400, 50) });
	entity_map.insert({ 85, Entity(-300, -400) });

	my_quadtree.insert(9);
	my_quadtree.insert(54);
	my_quadtree.insert(46);
	my_quadtree.insert(15);
	my_quadtree.insert(85);

	std::vector<int> ids;
	my_quadtree.retrieve(100, 100, 400, 400, ids);

	std::cout << "Found: " << ids.size() << " objects\n";
	for (auto id : ids) {
		std::cout << id << ", ";
	}

	return 0;
}