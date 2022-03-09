#pragma once
#include <vector>
#include <string>
#include <stdexcept>

struct Packet {
    uint16_t offset = 0;
    std::vector<char> m_buf;
    
    /**
     * Readers 
    */
    bool Read(void* out, size_t _Size);

    template<typename T>
    bool Read(T out) {
        return Read(&out, sizeof(out));
    }

    bool Read(std::string &out);

    /*
    * Writers
    */
    bool Write(void* in, size_t _Size);

    template<typename T>
    bool Write(T in) {
        return Write(&in, sizeof(in));
    }

    bool Write(std::string &in);
};