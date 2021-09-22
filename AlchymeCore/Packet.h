#pragma once
#include <vector>
#include <string>
#include <stdexcept>

struct Packet {
    std::size_t offset = 0;
    std::vector<char> m_buf;
    
    template<typename T>
    void Read(T &out) {
        //if (offset + sizeof(T) > m_buf.size())
        //    throw std::range_error("Reading out of bounds");

        std::memcpy(&out, m_buf.data() + offset, sizeof(T));

        offset += sizeof(T);
    }

    void Read(std::string &out);

    //template<typename T>
    //void Write(const char* in);

    template<typename T>
    void Write(T &in) {
        // should not use this, a big enough vec
        // should be passed in during construction

        //Write(&in);

        if (m_buf.size() < offset + sizeof(T)) {
            m_buf.resize(offset + sizeof(T));
        }

        std::memcpy(m_buf.data() + offset, &in, sizeof(T));        
        offset += sizeof(T);
    }

    void Write(std::string &in);

    //void Write(std::vector<char> buf);
};