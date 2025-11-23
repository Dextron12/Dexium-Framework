//
// Created by Dextron12 on 18/11/2025.
//

#ifndef DEXIUM_ID_HPP
#define DEXIUM_ID_HPP


#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <random>

namespace Dexium::Utils {
    class ID16 {
    public:
        ID16(const std::string& strID);

        ID16() = delete;
        ~ID16() = default;

        // Converts the internal binary format back to a string and returns it. Good for logging the ID
        std::string getStrID() const;

        //Displays the internal binary format in a hex style string (ASCII char + padding)
        std::string hexID() const;

        const std::array<uint8_t, 16>& getBytes() const;

        bool operator==(const ID16& other) const;
        bool operator!=(const ID16& other) const;

    private:
        std::array<uint8_t, 16> bytes = {0, 16}; // Stores each char (in binary) up to 16 chars. Needs to be converted to int fort streaming out (or it outputs as the char itself)
        int m_length = 0;
    };

    // This class is NOT created by Dexium devs or Dextron12.
    // Credit to: https://github.com/rkg82/uuid-v4
    class UUID
    {
    public:
        // Factory method for creating UUID object.
        static UUID New()
        {
            UUID uuid;
            std::random_device rd;
            std::mt19937 engine{rd()};
            std::uniform_int_distribution<int> dist{0, 256}; //Limits of the interval

            for (int index = 0; index < 16; ++index)
            {
                uuid._data[index] = (unsigned char)dist(engine);
            }

            uuid._data[6] = ((uuid._data[6] & 0x0f) | 0x40); // Version 4
            uuid._data[8] = ((uuid._data[8] & 0x3f) | 0x80); // Variant is 10

            return uuid;
        }

        bool operator==(const UUID& other) const {
            return std::memcmp(_data, other._data, 16) == 0;
        }

        bool operator!=(const UUID& other) const {
            return !(*this == other);
        }

        // Returns UUID as formatted string
        std::string String()
        {
            // Formats to "0065e7d7-418c-4da4-b4d6-b54b6cf7466a"
            char buffer[256] = {0};
            std::snprintf(buffer, 255,
                          "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                          _data[0], _data[1], _data[2], _data[3],
                          _data[4], _data[5],
                          _data[6], _data[7],
                          _data[8], _data[9],
                          _data[10], _data[11], _data[12], _data[13], _data[14], _data[15]);

            std::string uuid = buffer;

            return uuid;
        }

        std::string String() const {
            // Formats to "0065e7d7-418c-4da4-b4d6-b54b6cf7466a"
            char buffer[256] = {0};
            std::snprintf(buffer, 255,
              "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
              _data[0], _data[1], _data[2], _data[3],
              _data[4], _data[5],
              _data[6], _data[7],
              _data[8], _data[9],
              _data[10], _data[11], _data[12], _data[13], _data[14], _data[15]);

            std::string uuid = buffer;

            return uuid;
        }

    private:
        UUID() {}

        unsigned char _data[16] = {0};
    };




    class ID {
    public:
        // Generates a numeric/hashed IDTag. If a str is provided, the str is converted to binary and stored as an array of unt8_t for quciker comparisons compared to raw strings
        // If no strID is provided (default behaviour) a UUIDv4(random) is generated as the ID
        ID();
        ID(const std::string& strID);

        union IDStore {
            Dexium::Utils::ID16 id16; // Dexium binary str ID
            Dexium::Utils::UUID uuidValue; // An imported V4 uuid header

            IDStore() {}
            ~IDStore() {}

            IDStore(const IDStore&) = delete;
            IDStore& operator=(const IDStore&) = delete;
        };

        enum class IDType {
            ID16,       // Uses Dexium binary char ID
            UUID        // Imported V4 UUID class
        };

        ~ID() {
            destroy(); // Frees internal mem allocated with 'new'
        }

        // Copy ctor
        ID(const ID& other) : type (other.type) {
            copyFrom(other);
        }

        bool operator==(const ID& other) const;
        bool operator!=(const ID& other) const;

        // Compars between ID & ID16
        bool operator==(const ID16& other) const;
        bool operator==(const UUID& other) const;

        bool operator!=(const ID16& other) const;
        bool operator!=(const UUID& other) const;

        // Assignment
        ID& operator=(const ID& other);

        // If type = ID16 -> stringID(what was provided as initialisation). If type = UUID -> returns hashed UUID
        std::string str();

        // If type = ID16 -> ASCII (int) values + '0' padding(Hash-Lite). If type = UUID -> returns hashed UUID
        std::string hash();

        std::size_t numericHash() const;

    private:
        IDStore storage; // a union var that stores a UUID adn ID16. --> Faster than std::variant
        IDType type;

        void destroy();

        // Helper to copy the correct type
        void copyFrom(const ID& other);


    };


};

namespace std {
    template <>
    struct hash<Dexium::Utils::ID> {
        std::size_t operator()(const Dexium::Utils::ID& id) const noexcept {
            return id.numericHash();
        }
    };
}

#endif //DEXIUM_ID_HPP