//
// Created by Dextron12 on 18/11/2025.
//

#ifndef DEXIUM_ID_HPP
#define DEXIUM_ID_HPP


#include <array>
#include <cstdint>
#include <string>

namespace Dexium::Utils {

    class ID16 {
    public:
        ID16(const std::string& strID);

        // Converts the internal binary format back to a string and returns it. Good for logging the ID
        std::string getStrID() const;

        //Displays the internal binary format in a hex style string (ASCII char + padding)
        std::string hexID() const;

    private:
        std::array<uint8_t, 16> bytes = {0, 16}; // Stores each char (in binary) up to 16 chars. Needs to be converted to int fort streaming out (or it outputs as the char itself)
        int m_length = 0;
    };
}

#endif //DEXIUM_ID_HPP