#pragma once
#include <vector>

struct Packet {
    std::size_t offset = 0;
    std::vector<char> m_buf;
    
    template<typename T>
    T Read() {
        T out;
        std::memcpy(&out, m_buf.data() + offset, sizeof(T));
        offset += sizeof(T);
        return out;
    }

    template<typename T>
    void Write(T in) {
        // should not use this, a big enough vec
        // should be passed in during construction
        if (m_buf.size() < offset + sizeof(T)) {
            m_buf.resize(offset + sizeof(T) * 3);
        }
        std::memcpy(m_buf.data() + offset, &in, sizeof(T));
        offset += sizeof(T);
    }
};