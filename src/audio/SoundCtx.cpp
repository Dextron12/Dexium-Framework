//
// Created by Dextron12 on 27/10/2025.
//
#define MINIAUDIO_IMPLEMENTATION
#include <Audio/SoundCtx.hpp>
#include <core/VFS.hpp>

#include "Dexium.hpp"

void SoundEngine::initEngine() {
    if (ma_engine_init(nullptr, &get().m_engine) != MA_SUCCESS) {
        TraceLog(Dexium::LOG_ERROR, "[Audio]: Audio engine failed to initialize");
        return;
    }
    get().inited = true;
}

SoundEngine &SoundEngine::get() {
    static SoundEngine instance;
    return instance;
}

SoundEngine::~SoundEngine() {
    ma_engine_uninit(&m_engine);
}

SoundContext::SoundContext(const std::string& audioPath, bool loopAudio) {
    // Validate path
    auto absPath = VFS::resolve(audioPath);
    if (absPath) {
        //Store the abs path as its locator
        soundPath = absPath->string();
        // Check that it's extension matches a supported type
        std::string ext = absPath->extension().string();
        if (ext != ".wav" && ext != ".mp3" && ext != ".ogg" && ext != ".flac") {
            TraceLog(Dexium::LOG_ERROR, "[Audio Context]: '{}' is an unsupported audio file", audioPath); // Purposely use the relative path here
            fmt::print("Dexium Audio supports: \n"
                "- WAV"
                "- FLAC"
                "- OGG"
                "- MP3");
            inited = false;
            return; // Fail initialisation if invalid format detected
        }
        // Audio file exists and format is supported
    } else {
        TraceLog(Dexium::LOG_ERROR, "[Audio Context]: '{}' could not be found from the abs path: {}", audioPath, absPath->string());
        inited = false;
        return;
    }

    // Init configuration
    config = ma_sound_config_init();
    if (loopAudio) {
        config.flags |= MA_SOUND_FLAG_LOOPING;
        loop = true;
        // Bitwise oeprator '|=' --> add to bitmask
        // Bitwise operator '&= ~FLAG' --> remove specified flag from bitmask
    }
    // Audio path already validated. Store it in config
    config.pFilePath = soundPath.c_str();

    inited = true; // The parant implementatio/s for audio will handle everyhting else
}

SoundContext::~SoundContext() {
    // Deinit the sound buffer
    //ma_sound_uninit(&soundBuffer);
    //inited = false;
}



const char* ma_result_to_string(ma_result result) {
    switch (result) {
        case MA_SUCCESS: return "MA_SUCCESS";
        case MA_ERROR: return "MA_ERROR";
        case MA_INVALID_ARGS: return "MA_INVALID_ARGS";
        case MA_INVALID_OPERATION: return "MA_INVALID_OPERATION";
        case MA_OUT_OF_MEMORY: return "MA_OUT_OF_MEMORY";
        case MA_OUT_OF_RANGE: return "MA_OUT_OF_RANGE";
        case MA_ACCESS_DENIED: return "MA_ACCESS_DENIED";
        case MA_DOES_NOT_EXIST: return "MA_DOES_NOT_EXIST";
        case MA_ALREADY_EXISTS: return "MA_ALREADY_EXISTS";
        case MA_FORMAT_NOT_SUPPORTED: return "MA_FORMAT_NOT_SUPPORTED";
        case MA_DEVICE_TYPE_NOT_SUPPORTED: return "MA_DEVICE_TYPE_NOT_SUPPORTED";
        case MA_SHARE_MODE_NOT_SUPPORTED: return "MA_SHARE_MODE_NOT_SUPPORTED";
        case MA_NO_BACKEND: return "MA_NO_BACKEND";
        case MA_NO_DEVICE: return "MA_NO_DEVICE";
        case MA_API_NOT_FOUND: return "MA_API_NOT_FOUND";
        case MA_INVALID_FILE: return "MA_INVALID_FILE";
            //case MA_INVALID_FILE_FORMAT: return "MA_INVALID_FILE_FORMAT";
            //case MA_INVALID_FILE_VERSION: return "MA_INVALID_FILE_VERSION";
            //case MA_FILE_NOT_FOUND: return "MA_FILE_NOT_FOUND";
        case MA_TOO_MANY_OPEN_FILES: return "MA_TOO_MANY_OPEN_FILES";
        case MA_PATH_TOO_LONG: return "MA_PATH_TOO_LONG";
        default: return "UNKNOWN_ERROR";
    }
}

