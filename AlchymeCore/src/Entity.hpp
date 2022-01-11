#pragma once
#include <vector>
#include <Vec2.hpp>

// keep object inheritance very minimal

// so no virtual functions, no OOP
// focus on cpu cache and performance
// for simplicity

// data oriented design

enum class EntityType : uint8_t {
	PLAYER = 0,
};

struct Transform {
	float m_posX, m_posY;
	float m_velX, m_velY;
};

struct EntityContainer {
	//std::vector<Vec2f> m_pos;
	//std::vector<Vec2f> m_vel;
	std::vector<Transform> m_entityTransforms;
	std::vector<EntityType> m_entityTypes;
};