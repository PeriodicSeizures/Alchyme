#include "Packet.h"

bool Packet::Read(void* out, size_t _Size) {
    if (offset + _Size > m_buf.size())
        return false;

    //    destination, source
    std::memcpy(out, m_buf.data() + offset, _Size);

    offset += _Size;

    return true;
}

bool Packet::Read(std::string &out) {
    size_t _Size; if (!Read(&_Size, sizeof(_Size))) return false;

    if (_Size > 256)
        return false;

    out.resize(_Size);

    // Will never read null terminating character into string
    // Also will never expect null terminating character to finalize string
    return Read(out.data(), _Size);
}

bool Packet::Write(void* in, size_t _Size) {
    if (offset + _Size > m_buf.size())
        return false;

    //    destination, source
    std::memcpy(m_buf.data() + offset, in, _Size);

    offset += _Size;

    return true;
}

bool Packet::Write(std::string &in) {
    size_t _Size = in.size(); if (!Write(&_Size, sizeof(_Size))) return false;

    if (_Size > 256)
        return false;

    return Write(in.data(), _Size);
}