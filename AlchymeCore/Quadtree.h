#pragma once
#include <memory>
#include <vector>
#include <stdint.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include "Utils.h"

struct Pixel {
	unsigned char red, green, blue;
};

template<typename T, void (*NODE_CALLBACK)(int, T&, T&), int MAX_NODES = 7, int MAX_DEPTH = 6>
class Quadtree {
	typedef Quadtree<T, NODE_CALLBACK, MAX_NODES, MAX_DEPTH> QTree;
	typedef std::unique_ptr<QTree> ptr;
	//typedef void (*node_callback)(int, T&, T&);

	//union Leaf {
	//	std::unordered_set<int> nodes;
	//	Quadtree::ptr sub[4];
	//
	//	Leaf() : nodes() {}
	//};

	// Size of the 'Box'
	T m_xMin, m_yMin, m_xMax, m_yMax;
	//node_callback m_callback;
	int m_depth;

	// Contains simple uids of whether a 
	//Leaf m_leaf;
	std::unordered_set<int> m_nodes;
	ptr m_sub[4];
	std::unordered_set<int> m_allNodes;
	//bool m_isSubDivided = false;

	void subdivide() {
		T xOffset = m_xMin + (m_xMax - m_xMin) / 2;
		T yOffset = m_yMin + (m_yMax - m_yMin) / 2;

		// This section is correct ATM
		m_sub[0] = std::make_unique<QTree>(m_xMin, m_yMin, xOffset, yOffset, m_depth + 1);
		m_sub[1] = std::make_unique<QTree>(xOffset, m_yMin, m_xMax, yOffset, m_depth + 1);
		m_sub[2] = std::make_unique<QTree>(m_xMin, yOffset, xOffset, m_yMax, m_depth + 1);
		m_sub[3] = std::make_unique<QTree>(xOffset, yOffset, m_xMax, m_yMax, m_depth + 1);
	}

	void update() {
		//m_nodes.clear();


	}

	void collapse() {
		m_sub[0].reset();
		m_sub[1].reset();
		m_sub[2].reset();
		m_sub[3].reset();
	}

	void releaf() {
		// Move nodes as needed
	}

	bool isPointWithin(T x, T y) {
		return x >= m_xMin && x < m_xMax &&
			y >= m_yMin && y < m_yMax;
	}

	static bool isPointWithin(T x, T y, T xMin, T yMin, T xMax, T yMax) {
		return x >= xMin && x < xMax &&
			y >= yMin && y < yMax;
	}

	//		|---------------------| TREE
	//  |------------| CHECK
	// Selection rectangle needs to know whether quadtree is outside of it 
	bool anyOverlap(T xMin, 
					T yMin, 
					T xMax, 
					T yMax) {
		
		// Return whether THAT RECT and THIS RECT overlap

		// WHETHER
		return m_xMin < xMax &&
			m_xMax > xMin &&
			m_yMin < yMax &&
			m_yMax > yMin;
	}

	bool isInsideOf(T xMin,
		T yMin,
		T xMax,
		T yMax) {
		return m_xMin >= xMin &&
			m_xMax <= xMax &&
			m_yMin >= yMin &&
			m_yMax <= yMax;
	}

public:
	Quadtree(T xMin,
			 T yMin,
			 T xMax,
			 T yMax,
			 int depth = 0) // How far down this Quad is
		: m_xMin(xMin),
		  m_yMin(yMin),
		  m_xMax(xMax),
		  m_yMax(yMax),
		  m_depth(depth)
	{
		m_nodes.reserve(MAX_NODES);
	}
	
	/*
	* Insert a node into the tree, either through
	* removal and reinsertion if already present,
	* or Simple insertion.
	*/
	void insert(int node) {

		// If old node existed
		if (m_allNodes.contains(node)) {
			this->remove(node);
		}

		m_allNodes.insert(node);



		if (m_sub[0]) {
			T x, y;
			NODE_CALLBACK(node, x, y);
			for (auto&& sub : m_sub) {
				if (sub->isPointWithin(x, y)) {
					sub->insert(node);
					break;
				}
			}
			return;
		}

		m_nodes.insert(node);
		if (m_nodes.size() > MAX_NODES && m_depth < MAX_DEPTH) {
			// Realloc to insert in a subtree

			// split
			subdivide();
			//LOG_DEBUG("SUBDIVIDE\n");

			// R
			for (auto&& n : m_nodes) {
				T x, y;
				NODE_CALLBACK(n, x, y);
				for (auto&& sub : m_sub) {
					if (sub->isPointWithin(x, y)) {
						sub->insert(n);
						break;
					}
				}
			}

			m_nodes.clear();

			// Add node to next deepest tree 
			//if (m_sub[0]->isPointWithin(x, y))
			//	m_sub[0]->insert(node);
			//else if (m_sub[1]->isPointWithin(x, y))
			//	m_sub[1]->insert(node);
			//else if (m_sub[2]->isPointWithin(x, y))
			//	m_sub[2]->insert(node);
			//else if (m_sub[3]->isPointWithin(x, y))
			//	m_sub[3]->insert(node);
		}
	}

	bool remove(int node) {
		if (m_allNodes.erase(node)) {
			if (this->m_sub[0]) {
				return m_sub[0]->remove(node) ||
					m_sub[1]->remove(node) ||
					m_sub[2]->remove(node) ||
					m_sub[3]->remove(node);
			}
			// Else this is the deepest tree
		}
		return this->m_nodes.erase(node);
	}

	/*
	* Try to find a node located somewhere 
	* in the tree
	*/
	bool find(int node) {
		if (this->m_sub[0]) {
			return m_sub[0]->find(node) ||
				m_sub[1]->find(node) ||
				m_sub[2]->find(node) ||
				m_sub[3]->find(node);
		}
		// Else this is the deepest tree
		return this->m_nodes.contains(node);
	}

	void print() {
		if (m_sub[0]) {
			m_sub[0]->print();
			m_sub[1]->print();
			m_sub[2]->print();
			m_sub[3]->print();
		}
		else {
			// this is the deepest, then print
			std::cout << "NODES: ";
			for (auto node : m_nodes)
				std::cout << node << ", ";
			std::cout << "\n";
		}
	}

	/*
	* Now, the obvious intended purpose 
	* of using a QuadTree is gather all
	* objects within a certain spacial range (box, radius, ...)
	* 
	* And objects must be releafed as necessary for accuracy
	*/

	void retrieve(std::vector<int> &nodes) {
		nodes.insert(nodes.begin(), this->m_allNodes.begin(), this->m_allNodes.end());

		// If this has child Quads
		//if (m_sub[0]) {
		//	m_sub[0]->retrieve(nodes);
		//	m_sub[1]->retrieve(nodes);
		//	m_sub[2]->retrieve(nodes);
		//	m_sub[3]->retrieve(nodes);
		//}
		//else {
		//	nodes.insert(nodes.begin(), this->m_allNodes.begin(), this->m_allNodes.end());
		//}
	}

	void retrieve(T x1, T y1, T x2, T y2, std::vector<int>& nodes) {
		// Reserve ahead of time
		nodes.reserve(m_allNodes.size());

		LOG_DEBUG("RETRIEVE, all: %d, imm: %d\n", m_allNodes.size(), m_nodes.size());

		// If tree is completely inside
		if (isInsideOf(x1, y1, x2, y2)) {
			// Get all objects in tree
			retrieve(nodes);
			LOG_DEBUG("INSIDE OF\n");
		}
		else if (anyOverlap(x1, y1, x2, y2)) {
			// If there are SUBTREES, RECURSIVELY GET
			if (m_sub[0]) {
				m_sub[0]->retrieve(x1, y1, x2, y2, nodes);
				m_sub[1]->retrieve(x1, y1, x2, y2, nodes);
				m_sub[2]->retrieve(x1, y1, x2, y2, nodes);
				m_sub[3]->retrieve(x1, y1, x2, y2, nodes);
			}
			else {
				// Assumes that there are no generates nodes
				// for any node, add it

				// Then scan each id
				//LOG_DEBUG("FINAL-CASE, all: %d, imm: %d\n", m_allNodes.size(), m_nodes.size());
				for (auto&& node : this->m_allNodes) {
					T x, y;
					NODE_CALLBACK(node, x, y);
					LOG_DEBUG("OBJECT %d, (%f, %f)\n", node, x, y);
					if (isPointWithin(x, y, x1, y1, x2, y2))
						nodes.push_back(node);
				}

				//nodes.insert(nodes.begin(), this->m_allNodes.begin(), this->m_allNodes.end());
			}
		}
	}

	static void PIXEL_AT(int i, int j,
		unsigned char r, unsigned char g, unsigned char b,
		std::vector<Pixel>& framebuffer,
		int width, int height) 
	{
		int pitch = i + j * width;

		if (pitch < framebuffer.size())
			framebuffer[pitch] = { r, g, b };
	}


	//#define PIXEL_AT(x, y, r, g, b, fb, w, h) {int p = y*w + x; if (p < fb.size()) fb[p] = Pixel(r, g, b);}

	void draw(std::vector<Pixel> &framebuffer, int width, int height) {
		//for (size_t j = 0; j < height; j++) {
		//	for (size_t i = 0; i < width; i++) {
		//		if (i < 300)
		//			PIXEL_AT(i, j, 0, 255, 0, framebuffer, width, height);
		//	}
		//}
		//return;

		// iterate, appending data
		if (m_sub[0]) {
			// draw sub bounds
			for (auto&& sub : m_sub) {
				sub->draw(framebuffer, width, height);
			}
		}
		else {
			// draw nodes
			for (auto&& node : m_nodes) {
				T x, y;
				NODE_CALLBACK(node, x, y);
				PIXEL_AT(x, y, 40, 200, 200, framebuffer, width, height);
			}
		}
		
		// draw this bounds
		for (int x = m_xMin; x < m_xMax; x++) {
			PIXEL_AT(x, m_yMin, 20, 180, 20, framebuffer, width, height);
			PIXEL_AT(x, m_yMax, 20, 180, 20, framebuffer, width, height);
		}
		
		for (int y = m_yMin; y < m_yMax; y++) {
			PIXEL_AT(m_xMin, y, 20, 180, 20, framebuffer, width, height);
			PIXEL_AT(m_xMax, y, 20, 180, 20, framebuffer, width, height);
		}
	}

};