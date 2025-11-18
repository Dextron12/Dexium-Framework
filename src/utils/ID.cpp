//
// Created by Dextron12 on 18/11/2025.
//

#include <iomanip>
#include <utils/ID.hpp>

#include "core/Error.hpp"

#include <sstream>

Dexium::Utils::ID16::ID16(const std::string &strID) {
    if (strID.empty() || strID.size() > 16) {
        TraceLog(LOG_WARNING, "[ID]: The ID is of size {}. Cannot be empty or greater than 16 chars", strID.size());
        return;
    }

    m_length = strID.size();

    for (int i = 0; i < m_length; i++) {
        bytes[i] = static_cast<uint8_t>(strID[i]);
    }
}

 std::string Dexium::Utils::ID16::getStrID() const {
    std::string buff;
    for (int i = 0; i < m_length; i++) {
        buff += static_cast<char>(bytes[i]);
    }
    return buff;
}

std::string Dexium::Utils::ID16::hexID() const {
    std::string buff;
    for (int i = 0; i < m_length; i++) {
        std::stringstream conv; // Uses a stringstream for hex conversion, adds '0' padding if the ASCII value is one-difit.
        int dec = static_cast<int>(bytes[i]);
        conv << std::hex << std::setw(2) << std::setfill('0') << dec; // Enwsures each binary output is two chars long
        buff += conv.str();
    }
    return buff;
}