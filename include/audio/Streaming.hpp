//
// Created by Dextron12 on 27/10/2025.
//



#ifndef DEXIUM_STREAMING_HPP
#define DEXIUM_STREAMING_HPP

#include <string_view>

#include "SoundCtx.hpp"

class AudioStream {
public:
    AudioStream(std::string_view audioFile, bool loopAudio=false);

    // An optional function to explicitly load the audio file into memory. Calling play() on first use also invokes this function
    bool load();

    void play();
    void pause();
    void stop(); // Frees the audio buffer(memory) and after thsi fucntion is called, any subsequent play() calls will first need to reload the audio into memory(Isnt much of a problem when streaming)

    // Set's a volume level from 0(muted) to 100(as loud as the device supports)
    void setVolume(int volume);

    void toggleLooping(bool loopAudio=false);

private:
    SoundContext sound;

    bool playing = false;
    bool paused = false;

    ma_uint64 pausedFrame = 0; //Stores the loc of the cursor when reading the sound
};

#endif //DEXIUM_STREAMING_HPP