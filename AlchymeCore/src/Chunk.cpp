#include "Chunk.h"
#include "World.h"

World* world;

Chunk::Chunk(int x, int y) 
	: x(x), y(y) {

}

void Chunk::SetWorld(World* w) {
	world = w;
}
