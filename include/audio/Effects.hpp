//
// Created by Dextron12 on 28/10/2025.
//

#ifndef DEXIUM_EFFECTS_HPP
#define DEXIUM_EFFECTS_HPP

#include <string_view>
#include "SoundCtx.hpp"

class SoundEffect {
public:
    SoundEffect(std::string_view audioFile);
    ~SoundEffect() = default;

    bool load();
    void unload();

    void play();

private:
    SoundContext sound;

};

#endif //DEXIUM_EFFECTS_HPP