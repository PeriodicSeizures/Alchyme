#pragma once
#include <memory>
#include <vector>
#include <stdint.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>

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

		m_sub[0] = std::make_unique<QTree>(m_xMin, m_yMin, xOffset, yOffset, m_depth + 1);
		m_sub[1] = std::make_unique<QTree>(xOffset, m_yMin, xOffset, yOffset, m_depth + 1);
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

	//		|---------------------| TREE
	//  |------------| CHECK
	// Selection rectangle needs to know whether quadtree is outside of it 
	bool anyOverlap(	T xMin, 
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
			// At this point a node was successfully removed
			// So jsut continue...
		} else
			m_allNodes.insert(node);
		

		// Since function is recursive across multiple subtrees
		// when the deepest tree is reached, insert here
		// Will insert if space
		// 
		if (m_nodes.size() < MAX_NODES || m_depth == MAX_DEPTH) {
			m_nodes.insert(node);
		}
		else {
			// Realloc to insert in a subtree

			// split
			subdivide();

			T x, y;
			NODE_CALLBACK(node, x, y);

			// Add node to next deepest tree 
			if (m_sub[0]->isPointWithin(x, y))
				m_sub[0]->insert(node);
			else if (m_sub[1]->isPointWithin(x, y))
				m_sub[1]->insert(node);
			else if (m_sub[2]->isPointWithin(x, y))
				m_sub[2]->insert(node);
			else if (m_sub[3]->isPointWithin(x, y))
				m_sub[3]->insert(node);
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

	/*
	* Now, the obvious intended purpose 
	* of using a QuadTree is gather all
	* objects within a certain spacial range (box, radius, ...)
	* 
	* And objects must be releafed as necessary for accuracy
	*/

	void retrieve(std::vector<int> &nodes) {
		// If this has child Quads
		if (m_sub[0]) {
			m_sub[0]->retrieve(nodes);
			m_sub[1]->retrieve(nodes);
			m_sub[2]->retrieve(nodes);
			m_sub[3]->retrieve(nodes);
		}
		else {
			nodes.insert(nodes.begin(), this->m_allNodes.begin(), this->m_allNodes.end());
		}
	}

	void retrieve(T x1, T y1, T x2, T y2, std::vector<int>& nodes) {
		// Reserve ahead of time
		nodes.reserve(m_allNodes.size());

		// If tree is completely inside
		if (isInsideOf(x1, y1, x2, y2)) {
			// Get all objects in tree
			retrieve(nodes);
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
				nodes.insert(nodes.begin(), this->m_allNodes.begin(), this->m_allNodes.end());
			}
		}
	}


};