//
// Created by Dextron12 on 26/11/2025.
//

#ifndef DEXIUM_TEXTURECONFIG_HPP
#define DEXIUM_TEXTURECONFIG_HPP

namespace Dexium::Config {

    enum class FilterMode {
        Nearest,
        Linear
    };

    struct TextureConfig {
        FilterMode filterMode = FilterMode::Nearest;
        bool generateMips = false;
    };
}

#endif //DEXIUM_TEXTURECONFIG_HPP