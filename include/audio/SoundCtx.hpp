//
// Created by Dextron12 on 27/10/2025.
//

// Defines the context of sound in Dexium. Simply an audio with a path

#ifndef DEXIUM_SOUNDCTX_HPP
#define DEXIUM_SOUNDCTX_HPP

// Ensure MINIAUDIO_IMPLEMENTATION IS DEFINED SOMEHWERE
#include <miniaudio.h>
#include <string>

// A Debugger function to check ofr MiniAudio error's
const char* ma_result_to_string(ma_result result);

class SoundEngine {
public:
    static SoundEngine& get();
    static void initEngine();

private:
    SoundEngine() = default;
    ~SoundEngine();

    // Prevent copying
    SoundEngine(const SoundEngine&) = delete;
    SoundEngine& operator=(const SoundEngine&) = delete;

    friend class AudioStream;
    friend class SoundEffect;

    ma_engine m_engine;

    bool inited = false;
};

class SoundContext {
public:
    ma_sound soundBuffer; // The storage buffer for the sound (uisng miniaudio)
    ma_sound_config config; // The configuration for the audio. Be sure to configure its flags depending on its context

    std::string soundPath;

    bool loop = false;
    bool inited = false;
    bool firstPlay = true; // determines if the audio file should be loaded. If thsi si false, it is already laoded

    SoundContext(const std::string& audioPath, bool loopAudio=false);
    SoundContext() = default;
    ~SoundContext();
};

#endif //DEXIUM_SOUNDCTX_HPP