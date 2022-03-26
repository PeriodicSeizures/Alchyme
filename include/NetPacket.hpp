#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

namespace Alchyme {
    namespace Net {
        struct Packet {
            uint16_t offset = 0;
            std::vector<std::byte> m_buf;

            //uint16_t GetSize();

            /**
             * Readers
            */
            //bool Read(void* out, size_t size);    
            bool Read(std::byte* out, std::size_t size);

            bool Read(std::string& out);
            //bool Read(const std::string_view out);

            //template<typename T>
            //bool Read(T &out) {
            //    static_assert(!std::is_pointer<T>::value, "Cannot read into raw pointer");
            //    return Read(&out, sizeof(out));
            //}

            template<typename T>
            bool Read(T& out) requires std::is_trivially_copyable_v<T> {
                return Read(reinterpret_cast<std::byte*>(&out), sizeof(out));
            }

            /*
            * Writers
            */
            void Write(const std::byte*, std::size_t size);

            bool Write(const std::string_view str);

            template<typename T>
            void Write(const T in) requires std::is_trivially_copyable_v<T> {
                Write(reinterpret_cast<const std::byte*>(&in), sizeof(in));
            }
        };
    }
}
