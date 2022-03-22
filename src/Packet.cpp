#include "Packet.h"

//bool Packet::Read(void* out, size_t size) {
//    if (offset + size > m_buf.size())
//        return false;
//
//    //    destination, source
//    std::memcpy(out, m_buf.data() + offset, size);
//
//    offset += size;
//
//    return true;
//}

bool Packet::Read(std::string &out) {
    std::uint_fast8_t size; if (!Read(size)) return false;
    //if (size > 256)
    //    return false;

    if (size != 0) {
        out.resize(size);
        return Read(reinterpret_cast<std::byte*>(out.data()), size);
    }

    // Will never read null terminating character into string
    // Also will never expect null terminating character to finalize string
    
}

bool Packet::Read(std::byte* out, std::size_t size) {
    if (offset + size > m_buf.size())
        return false;
        //throw std::runtime_error("Buffer read overflow");
    std::memcpy(out, m_buf.data() + offset, size);
    offset += size;
    return true;
}

//bool Packet::Read(const std::string_view out) {
//    size_t size; if (!Read(size)) return false;
//    
//}



void Packet::Write(const std::byte* in, std::size_t size) {
    m_buf.insert(m_buf.end(), in, in + size);

    offset += size;
}

bool Packet::Write(const std::string_view str) {
    if (str.length() >= UINT8_MAX)
        return false;

    // Write length
    Write(static_cast<std::uint_fast8_t>(str.length()));
    if (str.length() != 0)
        Write(reinterpret_cast<const std::byte*>(&str[0]), str.length());
    return true;
}
