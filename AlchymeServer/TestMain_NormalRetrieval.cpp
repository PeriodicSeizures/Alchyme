#include <iostream>
#include <vector>
#include <chrono>

struct Entity {
	float x, y;

	bool isWithin(float cx, float cy, float radius) {
		auto dx = x - cx;
		auto dy = y - cy;
		return dx * dx + dy * dy <= radius * radius;
	}

	bool isWithin(float x1, float y1, float x2, float y2) {
		return x >= x1 && x < x2 && y >= y1 && y < y2;
	}
};

static std::vector<Entity> entities(10000000);

int main() {
	std::vector<int> retrieved;



	std::cout << "Inserting entities..." << std::endl;
	auto BEGIN_TIME = std::chrono::steady_clock::now();
	for (int i = 0; i < entities.size(); i++) {
		float x = 11 + (rand() % 1988);
		float y = 11 + (rand() % 1988);

		entities[i] = Entity(x, y);
		//std::cout << "id: " << i << ", (" << x << ", " << y << ")" << std::endl;
	}

	// Benchmark time to insert
	auto NOW = std::chrono::steady_clock::now();
	auto TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - BEGIN_TIME).count() / 1000.f;
	std::cout << "Inserted " << entities.size() << " entities over " << TIME_IT_TOOK << "s" << std::endl;
	std::cout << "- - - - -" << std::endl;

	float x1=1001, y1=1001, x2=1450, y2=1780;

	for (int i = 0; i < 50; i++) {
		auto BEGIN_TIME = std::chrono::steady_clock::now();
		for (int node = 0; node < entities.size(); node++) {
			auto&& e = entities[node];
			if (e.isWithin(x1, y1, x2, y2)) {
				retrieved.push_back(node);
			}
			//for (int onode = 0; onode < entities.size(); onode++) {
			//	auto&& other = entities[onode];
			//	if (e.isWithin(other.x, other.y, 100)) {
			//		retrieved.push_back(onode);
			//	}
			//}
		}

		auto NOW = std::chrono::steady_clock::now();
		auto TIME_IT_TOOK = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - BEGIN_TIME).count() / 1000.f;
		std::cout << "Took " << TIME_IT_TOOK << "s (trial " << i << ")" << std::endl;
	}

	std::cout << "Retrieved cumulative: " << retrieved.size() << "\n";

	return 0;
}