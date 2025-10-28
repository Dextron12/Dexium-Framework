//
// Created by Dextron12 on 28/10/2025.
//

#include <filesystem>
#include <Audio/SoundCtx.hpp>
#include <Audio/Effects.hpp>

#include <core/Error.hpp>

SoundEffect::SoundEffect(std::string_view audioFile) {
    //Check that the sound engine is inited
    if (!SoundEngine::get().inited) {
        SoundEngine::get().initEngine();
    }
    sound = SoundContext(audioFile.data());

    // Configure sound buffer for decode(Load into memory) outpyut
    sound.config.flags |= MA_SOUND_FLAG_DECODE;
}

// Load the sound into memory(use unload() to free it)
bool SoundEffect::load() {
    if (!sound.inited) return false;

    if (sound.firstPlay) {
        sound.config = ma_sound_config_init();
        sound.config.flags |= MA_SOUND_FLAG_DECODE;
        sound.config.pFilePath = sound.soundPath.c_str();

        //Configure sound buffer with re-inited config
        ma_result result = ma_sound_init_ex(&SoundEngine::get().m_engine, &sound.config, &sound.soundBuffer);
        if (result != MA_SUCCESS) {
            TraceLog(Dexium::LOG_ERROR, "[Audio]: Failed to init sound from file: {}", std::filesystem::path(sound.soundPath).stem().string());
            fmt::print("MA Error: {}", ma_result_to_string(result));
            return false;
        }

        sound.firstPlay = false;
        sound.inited = true;
    }

    return true;
}

void SoundEffect::play() {
    if (!sound.inited) return;

    //Play the sound (once)
    ma_sound_start(&sound.soundBuffer);
}

void SoundEffect::unload() {
    //Unlaod the sound data from memory. Can be rel;oaded calling load()
    if (!sound.inited) return;
    ma_sound_stop(&sound.soundBuffer);
    ma_sound_uninit(&sound.soundBuffer);
    sound.firstPlay = true;
}

