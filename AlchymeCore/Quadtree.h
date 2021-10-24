#pragma once
#include <memory>
#include <vector>
#include <stdint.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include "Utils.h"

/// Cpu or memory optimization
//#define OPT_FOR_MEM

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
#ifdef OPT_FOR_MEM
	std::unordered_set<int> m_nodes;
#else
	std::unordered_map<int, QTree*> m_nodes;
	QTree* m_parent;
#endif

	/// Subtree structure
	ptr m_sub[4];

	/// Nodes of this tree or nodes of subtree, recursively
	//std::unordered_set<int> m_allNodes;

	#define HAS_SUB (m_depth >> ((sizeof(m_depth)*8)-1))
	#define MARK_SUB (m_depth |= (0b1 <<((sizeof(m_depth)*8)-1)))
	#define UNMARK_SUB (m_depth &= ~(0b1 <<((sizeof(m_depth)*8)-1)))
	#define SUB_NOTNULL (m_sub[0] != NULL)

	/**
	 * @brief Try to split into 4 subtrees, depending on node count and depth
	 * @return Whether split was performed
	*/
	bool subdivide() {

		if (m_nodes.size() > MAX_NODES && m_depth < MAX_DEPTH) {

			if (SUB_NOTNULL) {
				T xOffset = m_xMin + (m_xMax - m_xMin) / 2;
				T yOffset = m_yMin + (m_yMax - m_yMin) / 2;

				m_sub[0] = std::make_unique<QTree>(m_xMin, m_yMin, xOffset, yOffset, m_depth + 1);
				m_sub[1] = std::make_unique<QTree>(xOffset, m_yMin, m_xMax, yOffset, m_depth + 1);
				m_sub[2] = std::make_unique<QTree>(m_xMin, yOffset, xOffset, m_yMax, m_depth + 1);
				m_sub[3] = std::make_unique<QTree>(xOffset, yOffset, m_xMax, m_yMax, m_depth + 1);
			}
			MARK_SUB;

			// Relocate all nodes
			for (auto&& n : m_nodes) {
				T x, y;
				#ifdef OPT_FOR_MEM
					NODE_CALLBACK(n, x, y);
					for (auto&& sub : m_sub) {
						if (sub->Iinsert(n))
							break;
					}
				#else
					NODE_CALLBACK(n.first, x, y);
					for (auto&& sub : m_sub) {
						if (sub->Iinsert(n.first))
							break;
					}
				#endif
			}

			// Clear immenent nodes
			#ifdef OPT_FOR_MEM
				m_nodes.rehash(1);
			#else
				m_nodes.clear();
			#endif

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
		UNMARK_SUB;

		#ifdef OPT_FOR_MEM
			for (auto&& sub : m_sub) {
				sub.reset();
			}
		#endif
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
			return NULL;

		#ifdef OPT_FOR_MEM
			m_nodes.insert(node);
		#endif

		if (HAS_SUB) {
			QTree* deepest;
			for (auto&& sub : m_sub) {
				if (deepest = sub->Iinsert(node)) {
					#ifdef OPT_FOR_MEM
					#else
						m_nodes.insert({ node, deepest });
					#endif
					return deepest;
				}
			}
			throw std::runtime_error("This scope should not be reached!\n");
		}

		subdivide();
		return this;
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

		return this->Iinsert(node);
	}

	/**
	 * @brief Internal use to remove all nodes through parents
	 * @param node 
	 * @return 
	*/
	void IremoveUp(int node) {
		if (m_nodes.erase(node)) {
			if (m_parent)
				m_parent->IremoveUp(node);
		}
	}

	/**
	 * @brief Remove a node from the tree or subtrees
	 * @param node The node
	 * @return Whether the node was present
	*/
	bool remove(int node) {
		// If node is anywhere in the tree
	#ifdef OPT_FOR_MEM
		if (m_nodes.erase(node)) {
	#else
		auto&& find = m_nodes.find(node);
		if (find != m_nodes.end()) {
			#endif
			// If theres a subtree
			if (HAS_SUB) {
				// If collapsing failed, then subnodes still exist
				// therefore remove the node in subs
				if (!collapse()) {
					#ifdef OPT_FOR_MEM
						for (auto&& sub : m_sub) {
							if (sub->remove(node))
								break;
						}
					#else
						// Go to the Tree pointed to by node, and remove all upstream
						find->second->IremoveUp(node);
					#endif
				}
				else // Erase it again, because it was readded by a successful collapse()
					m_nodes.erase(node);
			}
			return true;
		}
		else
			return false; // Else, its impossible to remove a node that was never present
	#endif
	}

	/**
	 * @brief
	*/
	void clear() {
		#ifdef OPT_FOR_MEM
			m_nodes.rehash(1);
		#else
			m_nodes.clear();
		#endif
		collapse();
	}

	bool contains(int node) {
		return m_nodes.contains(node);
	}

	/**
	 * @brief Get all nodes in this tree
	 * @param nodes The vector to add to
	*/
	void retrieve(std::vector<int> &nodes) {
		nodes.insert(nodes.begin(), this->m_nodes.begin(), this->m_nodes.end());
	}

	void retrieve(T x1, T y1, T x2, T y2, std::vector<int>& nodes) {
		// Reserve ahead of time
		nodes.reserve(m_nodes.size());

		//LOG_DEBUG("RETRIEVE, all: %d, imm: %d\n", m_allNodes.size(), m_nodes.size());

		// If tree is completely inside
		if (isInsideOf(x1, y1, x2, y2)) {
			// Get all objects in tree
			retrieve(nodes);
			//LOG_DEBUG("INSIDE OF\n");
		}
		else if (anyOverlap(x1, y1, x2, y2)) {
			// If there are SUBTREES, RECURSIVELY GET
			if (HAS_SUB) {
				for (auto&& sub : m_sub) {
					sub->retrieve(x1, y1, x2, y2, nodes);
				}
			}
			else {
				// Assumes that there are no generates nodes
				// for any node, add it

				// Then scan each id
				//LOG_DEBUG("FINAL-CASE, all: %d, imm: %d\n", m_allNodes.size(), m_nodes.size());
				for (auto&& node : this->m_nodes) {
					T x, y;
					NODE_CALLBACK(node, x, y);
					if (isPointWithin(x, y, x1, y1, x2, y2))
						nodes.push_back(node);
				}

				//nodes.insert(nodes.begin(), this->m_allNodes.begin(), this->m_allNodes.end());
			}
		}
	}

	void retrieve(T x, T y, T radius, std::vector<int>& nodes) {

		// return the objects found in a specific radius 

	}

	void print() {
		std::cout << "Tree contains: " << m_nodes.size() << " total subnodes\n";
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

	void render(std::string filename = "./out.ppm") {

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

		std::ofstream ofs; // save the framebuffer to file
		ofs.open(filename);
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
		//for (size_t j = 0; j < height; j++) {
		//	for (size_t i = 0; i < width; i++) {
		//		if (i < 300)
		//			PIXEL_AT(i, j, 0, 255, 0, framebuffer, width, height);
		//	}
		//}
		//return;



		// iterate, appending data
		if (HAS_SUB) {
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