#pragma once

#include <vector>

template<typename T>
class FastUnorderedList {
	size_t m_count;
	std::vector<T> m_vec;

public:
	FastUnorderedList() : m_count(0) { }

	// a b c nil nil				vec size: 5 count: 3
	// a b c						vec size: 3 count: 3
	void push_back(T element) {
		if (m_vec.size() > m_count)
			m_vec[m_count] = element;
		else {
			m_vec.push_back(element);
		}
		m_count++;
	}

	// a b c						vec size: 3 count: 3 
	// erase(2)
	void erase(size_t n) {
		if (m_count && n < m_count) {
			// swap the deleted element and the last element
			m_vec[n] = m_vec[--m_count];
		}
	}

	int size() const {
		return m_count;
	}

	void clear() {
		m_count = 0;
	}

	T& operator[](int n)
	{
		return elements[n];
	}

	const T& operator[](int n) const
	{
		return elements[n];
	}
};
