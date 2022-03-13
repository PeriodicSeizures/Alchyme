#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

struct Packet {
    uint16_t offset = 0;
    std::vector<char> m_buf;
    
    //uint16_t GetSize();

    /**
     * Readers 
    */
    bool Read(void* out, size_t _Size);

    template<typename T>
    bool Read(T &out) {
        static_assert(!std::is_pointer<T>::value, "Cannot read into raw pointer");
        return Read(&out, sizeof(out));
    }

    bool Read(std::string &out);

    /*
    * Writers
    */
    bool Write(void* in, size_t _Size);

    template<typename T>
    bool Write(T in) {
        static_assert(!std::is_pointer<T>::value, "Cannot write from raw pointer");
        return Write(&in, sizeof(in));
    }

    bool Write(std::string &in);
};