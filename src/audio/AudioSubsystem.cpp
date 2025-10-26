// Created by Dextron12 On 20/10/25
// Dexium Audio Subsystem (Streamer)

// A streamer should be used for audio that is > 60 seconds
// Use static.hpp for simple sounds

#include "audio/AudioSubsystem.hpp"
#include "core/Error.hpp"
#include "core/VFS.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_ENABLE_OGG
#include <iostream>
#include <miniaudio.h>

// Can the streamer have multiple objects on the same device?

Dexium::Audio::AudioSubsystem::AudioSubsystem() {
    device = alcOpenDevice(nullptr); // default device
    if (!device) {
        TraceLog(LOG_ERROR, "[Audio-Subsystem]: Failed to open a playback device");
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || alcMakeContextCurrent(context) == ALC_FALSE) {
        TraceLog(LOG_ERROR, "[Audio-Subsystem]: Failed to create context from device");
        return;
    }

    alGenSources(1, &source);
    alGenBuffers(4, buffers); // 4 channels
}

Dexium::Audio::AudioSubsystem::~AudioSubsystem() {
    stopStream();

    if (streamThread.joinable()) {
        streamThread.join();
    }

    alDeleteSources(1, &source);
    alDeleteBuffers(4, buffers);

    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }

    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
}

bool Dexium::Audio::AudioSubsystem::playStream(const std::string &filePath) {
    stopStream(); // stop anything from playing

    // resolve path
    auto path = VFS::resolve(filePath);
    if (!path) {
        TraceLog(LOG_ERROR, "[Audio-Subsystem]: Failed to resolve audio path '{}', resulting in '{}'", filePath, path->string());
        return false;
    }

    if (ma_decoder_init_file(path->string().c_str(), nullptr, &decoder) != MA_SUCCESS) {
        TraceLog(LOG_ERROR, "[Audio-Subsystem]: Failed to load audio from '{}'", path->string());
        return false;
    }

    playing = true;
    stopRequested = false;

    streamThread = std::thread(&AudioSubsystem::streamLoop, this);
    return true;
}

void Dexium::Audio::AudioSubsystem::pauseStream() {
    if (playing) {
        alSourcePause(source);
        playing = false;
    }
}

void Dexium::Audio::AudioSubsystem::stopStream() {
    stopRequested = true;
    playing = false;

    if (streamThread.joinable()) {
        streamThread.join();
    }

    alSourceStop(source);
    alSourcei(source, AL_BUFFER, 0);
    ma_decoder_uninit(&decoder);
}

void Dexium::Audio::AudioSubsystem::streamLoop() {
    constexpr size_t BUFFER_SAMPLES = 4096;
    std::vector<int16_t> pcm(BUFFER_SAMPLES * decoder.outputChannels);

    // Prefill buffers
    for (ALuint buf : buffers) {
        ma_uint64 framesRead = 0;
        ma_result result = ma_decoder_read_pcm_frames(&decoder, pcm.data(), BUFFER_SAMPLES, &framesRead);

        if (result != MA_SUCCESS) {
            TraceLog(LOG_ERROR, "[Audio]: Decoder failed to read PCM frames: {}", (char)result);
            break;
        }

        assert(framesRead <= BUFFER_SAMPLES);

        std::cout
        << "Frames Read: " << framesRead
        << " channels: " << decoder.outputChannels
        << " sampleRate: " << decoder.outputSampleRate
        << " PCM Size: " << pcm.size()
        << std::endl;

        if (framesRead == 0) {
            TraceLog(LOG_WARNING, "[Audio]: No PCM frames read, reached end of file?");
            break;
        }

        /*ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            std::cout << "AL error before bufferData: " << err << std::endl;
        }*/

        //Segfault Here!!!
        /*alBufferData(buf,
            decoder.outputChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
            pcm.data(),
            static_cast<ALsizei>(framesRead * decoder.outputChannels * sizeof(short)),
            decoder.outputSampleRate);
        */
    }

    alSourceQueueBuffers(source, 4, buffers);
    alSourcePlay(source);

    while (!stopRequested) {
        ALint processed = 0;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while (processed-- > 0) {
            ALuint buf;
            alSourceUnqueueBuffers(source, 1, &buf);

            ma_uint64 framesRead = 0;
            ma_result result = ma_decoder_read_pcm_frames(&decoder, pcm.data(), BUFFER_SAMPLES, &framesRead);
            if (result != MA_SUCCESS) {
                stopRequested = true;
                break;
            }

            alBufferData(buf,
                decoder.outputChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,\
                pcm.data(),
                static_cast<ALsizei>(framesRead * decoder.outputChannels * sizeof(short)),
                decoder.outputSampleRate);
            alSourceQueueBuffers(source, 1, &buf);
        }

        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && !stopRequested) {
            alSourcePlay(source);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    alSourceStop(source);
}

