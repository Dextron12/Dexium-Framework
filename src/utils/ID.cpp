//
// Created by Dextron12 on 23/12/25.
//

#include <algorithm>
#include <utils/ID.hpp>

#include <random>
#include <sstream>
#include <iomanip>

namespace Dexium::Utils {

    UUID::UUID()
        : m_bytes{} {}; // Invalid (all zero's)

    UUID UUID::Invalid() {
        return UUID{};
    }

    bool UUID::isValid() const {
        // Checks each bit of the array to see if it matches 0, if all amtch 0, array is invalid state
        return std::any_of(
            m_bytes.begin(), m_bytes.end(), [](std::uint8_t b) {return b != 0;}
            );
    }

    UUID UUID::Generate() {
        std::array<uint8_t, 16> bytes{};

        static thread_local std::mt19937_64 rng{std::random_device{}()};
        std::uniform_int_distribution<std::uint64_t> dist;

        std::uint64_t high = dist(rng);
        std::uint64_t low = dist(rng);

        for (int i = 0; i < 8; ++i) {
            bytes[i] = static_cast<std::uint8_t>((high >> (56 - i * 8)) & 0xFF);
            bytes[i + 8] = static_cast<std::uint8_t>((low >> (56 - i * 8)) & 0xFF);
        }

        // Version (4)
        bytes[6] = (bytes[6] & 0x0F) | 0x40;

        // Variant (10xx)
        bytes[8] = (bytes[8] & 0x3F) | 0x80;

        return UUID{bytes};
    }

    UUID::UUID(const std::array<std::uint8_t, 16> &bytes)
        : m_bytes(bytes) {}

    bool UUID::operator==(const UUID &other) const {
        return m_bytes == other.m_bytes;
    }

    bool UUID::operator!=(const UUID &other) const {
        return !(*this == other);
    }


    std::string UUID::str() const {
        if (!isValid()) {
            return "InvalidUUID";
        }

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');

        for (size_t i = 0; i < m_bytes.size(); ++i) {
            oss << std::setw(2) << static_cast<int>(m_bytes[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                oss << '-';
            }
        }

        return oss.str();
    }


    IDHandle::IDHandle(std::optional<std::string> label) {
        if (label.has_value()) {
            m_label = label.value();
            // ID is auto-generated on construction
        }
    }

    const UUID &IDHandle::id() {
        return m_id;
    }

    void IDHandle::setLabel(const std::string& label) {
        m_label = label;
    }

    std::string IDHandle::str() {
        if (m_label.empty()) {
            return m_id.str();
        } else {
            return m_label;
        }
    }




}