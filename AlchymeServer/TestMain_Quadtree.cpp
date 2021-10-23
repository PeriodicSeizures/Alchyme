#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Quadtree.h"

struct Entity {
	float x, y;
};

static std::unordered_map<int, Entity> entity_map;

static void my_callback(int id, float& x, float& y) {
	auto&& entity = entity_map[id];
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






	Quadtree<float, my_callback> my_quadtree(100.f, 100.f, 600.f, 600.f);

	int trials;
	std::cout << "Enter the trials: ";
	std::cin >> trials;

	srand(time(NULL));

	std::cout << "\nEntities: \n";
	for (int i = 0; i < trials; i++) {
		int id = rand() % 89999 + 10000;
		float x = (rand() % 500) + 100;
		float y = (rand() % 500) + 100;

		entity_map.insert({ id, Entity(x, y) });
		my_quadtree.insert(id);

		std::cout << "id: " << id << ", (" << x << ", " << y << ")\n";
	}



	//entity_map.insert({ 9, Entity(0, 0) });
	//entity_map.insert({ 54, Entity(100, 200) });
	//entity_map.insert({ 46, Entity(300, -300) });
	//entity_map.insert({ 15, Entity(-400, 50) });
	//entity_map.insert({ 85, Entity(-300, -400) });
	//
	//my_quadtree.insert(9);
	//my_quadtree.insert(54);
	//my_quadtree.insert(46);
	//my_quadtree.insert(15);
	//my_quadtree.insert(85);

	my_quadtree.print();

	std::cout << "\n";

	std::vector<int> ids;
	my_quadtree.retrieve(100, 100, 600, 600, ids);
	//my_quadtree.retrieve(-1001, -1001, 1001, 1001, ids);

	std::cout << "Found: " << ids.size() << " objects\n";
	for (auto id : ids) {
		std::cout << id << ", ";
	}


	const int width = 1024;
	const int height = 768;
	std::vector<Pixel> framebuffer(width * height);

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
				framebuffer[i + j * width] = { 0, 0, 0 };
		}
	}
	my_quadtree.draw(framebuffer, width, height);







	std::ofstream ofs; // save the framebuffer to file
	ofs.open("./out.ppm");
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height * width; i++) {
		auto px = framebuffer[i];
		ofs << px.red;
		ofs << px.green;
		ofs << px.blue;
	}
	ofs.close();



	return 0;
}