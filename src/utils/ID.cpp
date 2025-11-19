//
// Created by Dextron12 on 18/11/2025.
//

#include <iomanip>
#include <utils/ID.hpp>

#include "core/Error.hpp"

#include <sstream>

#include "core/helpers.hpp"

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

bool Dexium::Utils::ID16::operator==(const ID16& other) const {
    //first, check if lenghts match:
    if (m_length != other.m_length) return false; // earlye xist, no match found if differ in size

    for (size_t i = 0; i < m_length; i++) {
        if (bytes[i] != other.bytes[i]) return false;
    }

    return true; // If no other logic block is executed, then they match
}

bool Dexium::Utils::ID16::operator!=(const ID16& other) const {
    return !(*this == other);
}

Dexium::Utils::ID::ID(const std::string& strID) : type(IDType::ID16) {
    if (strID.empty()) {
        type = IDType::UUID;
        new (&storage.uuidValue) UUID(UUID::New());
    } else {
        new (&storage.id16) ID16(strID);
    }
}

Dexium::Utils::ID::ID() : type(IDType::UUID) {
    new (&storage.uuidValue) UUID(UUID::New());
}

Dexium::Utils::ID& Dexium::Utils::ID::operator=(const ID &other) {
    if (this != &other) {
        destroy();
        type = other.type;
        copyFrom(other);
    }
    return *this;
}

bool Dexium::Utils::ID::operator==(const ID& other) const {
    if (type != other.type) return false; // Check type compat

    if (type == IDType::ID16) {
        if (storage.id16 != other.storage.id16) return false;
    } else {
        if (storage.uuidValue != other.storage.uuidValue) return false;
    }

    return true;
}

bool Dexium::Utils::ID::operator==(const ID16& other) const {
    if (type != IDType::ID16) return false; // Cannot compare uuid to ID16

    if (storage.id16 != other) return false;

    return true;
}

bool Dexium::Utils::ID::operator!=(const ID& other) const {
    return !(*this == other);
}

bool Dexium::Utils::ID::operator!=(const ID16& other) const {
    return !(*this == other);
}

bool Dexium::Utils::ID::operator==(const UUID& other) const {
    if (type != IDType::UUID) return false;

    if (storage.uuidValue != other) return false;

    return true;
}

void Dexium::Utils::ID::destroy() {
    if (type == IDType::ID16) {
        storage.id16.~ID16();
    } else {
        storage.uuidValue.~UUID();
    }
}

void Dexium::Utils::ID::copyFrom(const ID& other) {
    if (other.type == IDType::ID16) {
        new (&storage.id16) ID16(other.storage.id16);
    } else {
        new (&storage.uuidValue) UUID(other.storage.uuidValue);
    }
}

std::string Dexium::Utils::ID::str() {
    if (type == IDType::ID16) {
        return storage.id16.getStrID();
    } else {
        return storage.uuidValue.String();
    }
}

std::string Dexium::Utils::ID::hash(){
    if (type == IDType::ID16) {
        return storage.id16.hexID();
    }

    return storage.uuidValue.String();
}


