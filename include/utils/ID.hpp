//
// Created by Dextron12 on 23/12/25.
//

#ifndef DEXIUM_ID_HPP
#define DEXIUM_ID_HPP

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace Dexium::Utils {

    // A V4 UUID implementation
    class UUID {
    public:
        UUID(); // Generates invalid UUID V4 object. Call ::Generate() for valid assignment
        explicit UUID(const std::array<std::uint8_t, 16>& bytes);

        const std::array<std::uint8_t, 16>& bytes() const { return m_bytes; }

        bool operator==(const UUID&) const;
        bool operator!=(const UUID&) const;

        // Create an invlaid state of UUID
        static UUID Invalid();

        bool isValid() const;

        // Generate a valid UUID
        static UUID Generate();

        std::string str() const;

    private:
        std::array<std::uint8_t, 16> m_bytes{};
    };

    // A storage container for ID + optional label

    class IDHandle {
    public:
        IDHandle(std::optional<std::string> label = std::nullopt);

        //Fetches the label, or if not provided, fetches the hashed ID(str)
        std::string str();

        //Fetches the internally stored ID
        const UUID& id();

        void setLabel(const std::string& label);

    private:
        UUID m_id = UUID::Generate();
        std::string m_label{};
    };
}


// Hash support for UUID
namespace std {
    template <>
    struct hash<Dexium::Utils::UUID> {
        size_t operator()(const Dexium::Utils::UUID& uuid) const noexcept {
            const auto& b = uuid.bytes();
            size_t h = 0;
            for (auto byte : b) {
                h = (h * 131) + byte;
            }
            return h;
        }
    };
}


#endif //DEXIUM_ID_HPP