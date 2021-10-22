#pragma once
#include <memory>
#include <vector>
#include <stdint.h>

template<typename T, int maxNodes, int maxDepth, typename T>
class Quadtree {
	typedef std::unique_ptr<Quadtree> ptr;

	union Leaf {
		std::vector<Node> m_nodes;
		Quadtree::ptr m_subTree[4];
	};

	struct Node {
		T t;
		//std::vector<T> m_nodes;
		int8_t x;
		int8_t y;
	};

	Quadtree *m_parentNode;
	// if (m_depth & 0b10000000) == 0b10000000, then there are children leafs
	uint8_t m_depth;
	Leaf m_leaf;

	void diverge() {
		// if max bit is 0, there are no children
		// so split
		if ((m_depth & 0b10000000) == 0b00000000) {
			// get vector data before garbage values overwrite
			const Node* nodes = m_leaf.m_nodes.data();
			size_t count = m_leaf.m_nodes.size();

			m_leaf.m_nodes[0] = std::make_unique<Quadtree<maxNodes, maxDepth>>(depth);
			m_leaf.m_nodes[1] = std::make_unique<Quadtree<maxNodes, maxDepth>>(depth);
			m_leaf.m_nodes[2] = std::make_unique<Quadtree<maxNodes, maxDepth>>(depth);
			m_leaf.m_nodes[3] = std::make_unique<Quadtree<maxNodes, maxDepth>>(depth);

			// now releaf
		}

	}

	void converge() {

	}

	void releaf() {

	}

	Quadtree(int parentDepth) : depth(parentDepth + 1) {

	}

public:
	Quadtree() 
		: depth(0) {}
	
	void insert(T node, int8_t x, int8_t y) {



		// insert a node at x, y, where
		// x, y are absolute (non scaled; not relative)
		// so x, y must be downscaled for interpretation

		if (m_nodes.size() == count &&
			m_depth != depth) {
			// split
			diverge();
			// get the left to insert onto

			// insert into a node
			m_innerTree[]
		}
		else {
			nodes.push_back(node);
		}
	}

};