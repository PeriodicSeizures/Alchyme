#pragma once
#include <memory>
#include <vector>
#include <stdint.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include "Utils.h"

static void itll_callback(int id, float& x, float& y) {}

struct Pixel {
	unsigned char red, green, blue;
};

template<typename T, void (*NODE_CALLBACK)(int, T&, T&), int MAX_NODES = 7, uint8_t MAX_DEPTH = 6>
class Quadtree {
	static_assert(MAX_DEPTH < 128, "Max depth exceeds 127");
	static_assert(std::is_scalar<T>::value, "T must be a numeric type; ie float, int, double...");

	typedef Quadtree<T, NODE_CALLBACK, MAX_NODES, MAX_DEPTH> QTree;
	typedef std::unique_ptr<QTree> ptr;

	/// Boundaries of this tree
	T m_xMin, m_yMin, m_xMax, m_yMax;

	/// Recursive depth of this tree
	uint8_t m_depth = 0;

	/// All nodes in this or sub structures
	std::unordered_map<int, QTree*> m_nodes;
	QTree* m_parent;

	/// Subtree structure
	ptr m_sub[4];

	/// Nodes of this tree or nodes of subtree, recursively
	//std::unordered_set<int> m_allNodes;

	#define HAS_SUB ((m_depth >> ((sizeof(m_depth)*8)-1)) & 0b1)
	#define MARK_SUB (m_depth |= (0b1 <<((sizeof(m_depth)*8)-1)))
	#define UNMARK_SUB (m_depth &= ~(0b1 <<((sizeof(m_depth)*8)-1)))
	#define SUB_ISNULL (m_sub[0] == NULL)

	/**
	 * @brief Try to split into 4 subtrees, depending on node count and depth
	 * @return Whether split was performed
	*/
	bool subdivide() {

		if (m_nodes.size() > MAX_NODES && m_depth < MAX_DEPTH) {

			if (SUB_ISNULL) {
				T xOffset = m_xMin + (m_xMax - m_xMin) / 2;
				T yOffset = m_yMin + (m_yMax - m_yMin) / 2;

				m_sub[0] = std::make_unique<QTree>(m_xMin, m_yMin, xOffset, yOffset, m_depth + 1, this);
				m_sub[1] = std::make_unique<QTree>(xOffset, m_yMin, m_xMax, yOffset, m_depth + 1, this);
				m_sub[2] = std::make_unique<QTree>(m_xMin, yOffset, xOffset, m_yMax, m_depth + 1, this);
				m_sub[3] = std::make_unique<QTree>(xOffset, yOffset, m_xMax, m_yMax, m_depth + 1, this);
			}
			MARK_SUB;
			


			// Relocate all nodes
			for (auto&& n : m_nodes) {
				// Insert initial time without checking since it is guaranteed that the node belongs somewhere in this tree
				IinsertNoCheck(n.first);
			}

			//m_nodes.clear();

			return true;
		}
		return false;
	}
	
	/**
	 * @brief 
	*/
	void update() {
		// Update will move nodes to their correct tree,
		// assuming positions are constantly changing
		
		// An easy (but unoptimized) approach would be to 
		// delete subtrees and nodes (but keep m_allNodes)
		// and reinsert each node present in m_allNodes

		// An optimized approach would be to avoid any deep 
		// recursion, and any set node locating,
		// and INSTEAD:
		//	- For every node present in m_allNodes, 
		//	just try reinserting it
		// this will: 
		//  -> FIND and REMOVE ANY INSTANCES OF node in subtree
		//  -> INSERT into CORRECT TREE
		
		// Insertion will never collapse a tree, but could 
		// incur a subdivide
		
		//----------
		// Small overhead tree design:
		//	-> PROS:
		//		- Collapses subtrees when needed
		//	-> CONS:
		//		- More frequent tree allocations

		//----------
		// An optimal tree design might take options
		//  -> For large trees
		//		- 

		// So, ... when to collapse?
		// <On removal>
	}
	
	/**
	* Mark the subtrees as inactive
	* 
	* @param deleteSubs Delete subtree instead of only marking
	*/
	bool collapse() {
		if (HAS_SUB)
			if (m_nodes.size() <= MAX_NODES) {
				UNMARK_SUB;
				// Clear all sub nodes
				for (auto&& sub : m_sub) {
					sub->m_nodes.clear();
				}
				return true;
			}
		return false;
	}

	/**
	 * @brief If a given point is located in bounds
	 * @param x The x coordinate
	 * @param y The y coordinate
	 * @return If within
	*/
	bool isPointWithin(T x, T y) {
		return x >= m_xMin && x < m_xMax &&
			y >= m_yMin && y < m_yMax;
	}

	static bool isPointWithin(T x, T y, T xMin, T yMin, T xMax, T yMax) {
		return x >= xMin && x < xMax &&
			y >= yMin && y < yMax;
	}

	static bool isPointWithin(T x, T y, T cx, T cy, T r) {
		auto dx = x - cx;
		auto dy = y - cy;

		return dx * dx + dy * dy <= r* r;
	}

	/**
	 * @brief Determine if bounds overlap with a rectangle
	 * @param xMin Smallest x of rectangle
	 * @param yMin Smallest y of rectangle
	 * @param xMax Largest x of rectangle
	 * @param yMax Largest y of rectangle
	 * @return Whether there's an overlap
	*/
	bool anyOverlap(T xMin, 
					T yMin, 
					T xMax, 
					T yMax) {
		return m_xMin < xMax &&
			m_xMax > xMin &&
			m_yMin < yMax &&
			m_yMax > yMin;
	}

	/**
	 * @brief Determine if bounds lie exclusively within a rectangle
	 * @param xMin Smallest x of rectangle
	 * @param yMin Smallest y of rectangle
	 * @param xMax Largest x of rectangle
	 * @param yMax Largest y of rectangle
	 * @return Whether is inside
	*/
	bool isInsideOf(T xMin,
					T yMin,
					T xMax,
					T yMax) {
		return m_xMin >= xMin &&
			m_xMax <= xMax &&
			m_yMin >= yMin &&
			m_yMax <= yMax;
	}

	QTree* IinsertNoCheck(int node) {
		

		if (HAS_SUB) {
			QTree* deepest;
			for (auto&& sub : m_sub) {
				if (deepest = sub->Iinsert(node)) {
					m_nodes.insert({ node, deepest }); // Recursive returned value
					return deepest;
				}
			}
			throw std::runtime_error("This scope should not be reached!\n");
		}

		m_nodes.insert({ node, this });

		subdivide();

		// The 2nd real returned value
		return this;
	}

	/**
	 * @brief Insertion for internal use (is optimized by using constant behaviour assumptions)
	 * @param node 
	 * @return The deepest tree the node was inserted into
	*/
	QTree* Iinsert(int node) {

		// Check if node can fit here
		T x, y;
		NODE_CALLBACK(node, x, y);
		if (!this->isPointWithin(x, y))
			return NULL; // The 1st real returned value

		return this->IinsertNoCheck(node);
	}

public:
	Quadtree(T xMin,
			 T yMin,
			 T xMax,
			 T yMax,
			 int depth = 0,
			 QTree* parent = NULL) // How far down this Quad is
		: m_xMin(xMin),
		  m_yMin(yMin),
		  m_xMax(xMax),
		  m_yMax(yMax),
		  m_depth(depth),
		  m_parent(parent)
	{
		m_nodes.reserve(MAX_NODES);
	}
	
	/*
	* Insert a node into the tree, either through
	* removal and reinsertion if already present,
	* or Simple insertion.
	*/
	bool insert(int node) {
		// Remove any traces of old node
		this->remove(node);

		//m_nodes.insert({ node, this });

		return this->Iinsert(node);
	}

	/**
	 * @brief Remove a node from the tree or subtrees
	 * @param node The node
	 * @return Whether the node was present
	*/
	bool remove(int node) {
		// Find the deepest Tree of this node
		// Get ready to remove it
		auto&& find = m_nodes.find(node);
		if (find != m_nodes.end()) {
			collapse();
			//std::cout << "Removed duplicate\n";
			m_nodes.erase(node);

			return true;
		}
		else
			return false; // Else, its impossible to remove a node that was never present
	}

	/**
	 * @brief
	*/
	void clear() {
		m_nodes.clear();
		collapse();
	}

	bool contains(int node) {
		return m_nodes.contains(node);
	}

	size_t size() {
		return m_nodes.size();
	}

	/**
	 * @brief Get all nodes in this tree
	 * @param nodes The vector to add to
	*/
	void retrieve(std::vector<int> &nodes) {
		for (auto&& node : this->m_nodes)
			nodes.push_back(node.first);
	}

	void retrieve(T x1, T y1, T x2, T y2, std::vector<int>& nodes) {
		// Reserve for max possible nodes to add
		nodes.reserve(this->m_nodes.size());

		// If tree is completely inside selection
		if (isInsideOf(x1, y1, x2, y2)) {
			// Return everything
			retrieve(nodes);
		}
		else if (anyOverlap(x1, y1, x2, y2)) {
			// If even a slight touch, scan subs and self
			if (HAS_SUB) {
				for (auto&& sub : m_sub) {
					sub->retrieve(x1, y1, x2, y2, nodes);
				}
			}
			else {
				// Scan each node
				for (auto&& node : this->m_nodes) {
					T x, y;
					NODE_CALLBACK(node.first, x, y);
					if (isPointWithin(x, y, x1, y1, x2, y2))
						nodes.push_back(node.first);
				}
			}
		}
	}
	








	// insets
	bool isInsideOf(T x, T y, T r) {
		auto dx = std::max(x - m_xMin, m_xMax - x);
		auto dy = std::max(y - m_yMin, m_yMax - y);

		return r * r >= dx * dx + dy * dy;
	}

	bool anyOverlap(float x, float y, float r) {
		auto Xn = std::max(m_xMin, std::min(x, m_xMax));
		auto Yn = std::max(m_yMin, std::min(y, m_yMax));

		auto dx = Xn - x;
		auto dy = Yn - y;

		return (dx * dx + dy * dy) <= r * r;
	}

	void retrieve(T cx, T cy, T radius, std::vector<int>& nodes) {
		// Firstly, get all subs within radius
		nodes.reserve(this->m_nodes.size());
	
		if (isInsideOf(cx, cy, radius)) {

			// Then return all
			retrieve(nodes);

		}
		else if (anyOverlap(cx, cy, radius)) {
			if (HAS_SUB) {
				for (auto&& sub : m_sub) {
					sub->retrieve(cx, cy, radius, nodes);
				}
			}
			else {
				// Scan each node
				for (auto&& node : this->m_nodes) {
					T x, y;
					NODE_CALLBACK(node.first, x, y);
					if (isPointWithin(x, y, cx, cy, radius))
						nodes.push_back(node.first);
				}
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

	void render(/*std::vector<int>& retrieved*/) {

		LOG_DEBUG("RENDERING\n");

		const int width = 2560; // 1280; // m_xMax - m_xMin + 200;
		const int height = 2560; // 720; // m_yMax - m_yMin + 200;
		std::vector<Pixel> framebuffer(width * height);

		for (size_t j = 0; j < height; j++) {
			for (size_t i = 0; i < width; i++) {
				framebuffer[i + j * width] = { 0, 0, 0 };
			}
		}

		draw(framebuffer, width, height);


		//std::unordered_set<int> retrieved_set(retrieved.begin(), retrieved.end());
		for (auto&& node : m_nodes) {
			Pixel color = {40, 200, 200};

			T x, y;			
			NODE_CALLBACK(node.first, x, y);

			//if (retrieved_set.contains(node.first))
			//	color = {200, 40, 40};
			

			PIXEL_AT(x, y, color.red, color.green, color.blue, framebuffer, width, height);

			PIXEL_AT(x+1, y, color.red, color.green, color.blue, framebuffer, width, height);
			PIXEL_AT(x, y+1, color.red, color.green, color.blue, framebuffer, width, height);
			PIXEL_AT(x-1, y, color.red, color.green, color.blue, framebuffer, width, height);
			PIXEL_AT(x, y-1, color.red, color.green, color.blue, framebuffer, width, height);
		}

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
	}

	void draw(std::vector<Pixel> &framebuffer, int width, int height) {
		// iterate, appending data
		if (HAS_SUB) {
			// draw sub bounds
			for (auto&& sub : m_sub) {
				sub->draw(framebuffer, width, height);
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