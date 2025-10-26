// Created by Dextron12 On 20/10/25
// Dexium Audio Subsystem

// The audio subsystem is required to run on a seperate thread
// from the Dexium renderer as it uses thread sleep cycles to prevent
// extensive polling and CPU usage

#ifndef DEXIUM_AUDIO_SUBSYSTEM
#define DEXIUM_AUDIO_SUBSYSTEM

#include <AL/al.h>
#include <AL/alc.h>

#include <miniaudio.h>

#include <thread>
#include <atomic>
#include <vector>

#include "core/Error.hpp"



namespace Dexium::Audio {

    class AudioSubsystem {
    public:
        AudioSubsystem();
        ~AudioSubsystem();

        bool playStream(const std::string& filePath);
        void pauseStream();
        void stopStream();

    private:
        void streamLoop();

        ALCdevice* device;
        ALCcontext* context;

        ALuint source = 0;
        ALuint buffers[4] = {0};

        ma_decoder decoder{};
        std::thread streamThread; // Seperate thread to Dexium::OpenGL Context
        std::atomic<bool> playing{false};
        std::atomic<bool> stopRequested{false};
    };
}


#endif
