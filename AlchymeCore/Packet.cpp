#include "Packet.h"

//void Packet::WriteBytes(std::vector<char> buf) {
//	if (m_buf.size() < offset + buf.size()) {
//		m_buf.resize(offset + buf.size());
//	}
//	
//	std::memcpy(m_buf.data() + offset, buf.data(), buf.size());
//	offset += buf.size();
//}

void Packet::Read(std::string &out) {
    size_t size; Read(size); out.resize(size);
    std::memcpy(out.data(), m_buf.data() + offset, size);
    offset += size;
}

void Packet::Write(std::string &in) {
    // should not use this, a big enough vec
    // should be passed in during construction

    size_t size = in.size();

    Write(size);

    if (m_buf.size() < offset + size) {
        m_buf.resize(offset + size);
    }

    std::memcpy(m_buf.data() + offset, in.data(), size);
    offset += size;
}