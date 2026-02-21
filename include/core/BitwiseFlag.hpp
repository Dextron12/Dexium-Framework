//
// Created by ethan on 10/2/26.
//

#ifndef DEXIUM_BITWISEFLAG_HPP
#define DEXIUM_BITWISEFLAG_HPP

#include <type_traits>

// Defines a standard bitwise flag enumeration system
// Is dynamically applied to various enums adn systems throughout Dexium
// This header essentially defines a template for such a system

namespace Dexium::Core {

    template<typename E>
    struct EnableBitmaskOperators {
        static constexpr bool value = false;
    };

    //Define the Bitwise operators

    //OR
    template<typename E>
    constexpr std::enable_if_t<EnableBitmaskOperators<E>::value, E>
    operator|(E lhs, E rhs) {
        using U = typename std::underlying_type<E>::type;
        // Convert the operation to the underlying type(U) then return the enum type(E)
        return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
    }

    // AND
    template<typename E>
    constexpr std::enable_if_t<EnableBitmaskOperators<E>::value, E>
    operator&(E lhs, E rhs) {
        using U = typename std::underlying_type<E>::type;
        return static_cast<E>(
            static_cast<U>(lhs) & static_cast<U>(rhs)
        );
    }

    // NOT
    template<typename E>
    constexpr std::enable_if_t<EnableBitmaskOperators<E>::value, E>
    operator~(E v) {
        using U = typename std::underlying_type<E>::type;
        return static_cast<E>(~static_cast<U>(v));
    }

    // OR assignment
    template<typename E>
    constexpr std::enable_if_t<EnableBitmaskOperators<E>::value, E&>
    operator|=(E& lhs, E rhs) {
        lhs = lhs | rhs;
        return lhs;
    }

    // AND assignment
    template<typename E>
    constexpr std::enable_if_t<EnableBitmaskOperators<E>::value, E&>
    operator&=(E& lhs, E rhs) {
        lhs = lhs & rhs;
        return lhs;
    }

    // Helper func for checking if a Bitwise enum class has a set flag
    // - the first param should be the enum class var, seocnd should be the flag your checking
    template<typename E>
    constexpr std::enable_if_t<EnableBitmaskOperators<E>::value, bool>
    hasFlag(E value, E flag) {
        return (value & flag) == flag;
    }
}

#endif //DEXIUM_BITWISEFLAG_HPP