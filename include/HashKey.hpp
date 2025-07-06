#pragma once

#include <functional>
#include <tuple>

enum Layer {
	Ground,
	Decor,
	Structure,
	Enemy
};

namespace std {
    template<>
    struct hash<Layer> {
        std::size_t operator()(const Layer& layer) const {
            return std::hash<int>()(static_cast<int>(layer));
        }
    };

    template<>
    struct hash<std::tuple<int, int, Layer>> {
        std::size_t operator()(const std::tuple<int, int, Layer>& t) const {
            size_t h1 = std::hash<int>()(std::get<0>(t));
            size_t h2 = std::hash<int>()(std::get<1>(t));
            size_t h3 = std::hash<Layer>()(std::get<2>(t));
            return h1 ^ (h2 << 1) ^ (h3 << 2);  // or use a better mix if needed
        }
    };
}