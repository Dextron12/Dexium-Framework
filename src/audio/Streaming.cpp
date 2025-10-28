//
// Created by Dextron12 on 27/10/2025.
//

#include <filesystem>


#include <Audio/SoundCtx.hpp>
#include <Audio/Streaming.hpp>

#include <core/Error.hpp>


AudioStream::AudioStream(std::string_view audioFile, bool loopAudio) {
    if (!SoundEngine::get().inited) {
        SoundEngine::get().initEngine();
    }
    sound = SoundContext(audioFile.data(), loopAudio);

    // Configure internal flags for stream output
    sound.config.flags |= MA_SOUND_FLAG_STREAM; //MA_SOUNBD_FKLAG_DECODE --> load entire song into memory
    if (loopAudio) {
        sound.config.flags |= MA_SOUND_FLAG_LOOPING;
        sound.loop = true;
    }
}


/*
bool AudioStream::load() {
    if (sound.inited) {
        if (sound.firstPlay) {
            ma_result result = ma_sound_init_ex(&SoundEngine::get().m_engine, &sound.config, &sound.soundBuffer);
            if (result != MA_SUCCESS){
                fmt::print("{}\n", sound.soundPath);
                TraceLog(Dexium::LOG_ERROR, "[Audio]: Failed to init audio from file: {}", std::filesystem::path(sound.soundPath).stem().string());
                fmt::print("MA_Error: {}\n", ma_result_to_string(result));
                return false;
            }
            sound.firstPlay = false;
            playing = true;
        }
    }
    return true;
}
*/

bool AudioStream::load() {
    if (!sound.inited) return false;

    if (sound.firstPlay) {
        sound.config = ma_sound_config_init();
        sound.config.flags |= MA_SOUND_FLAG_STREAM;
        if (sound.loop) {
            sound.config.flags |= MA_SOUND_FLAG_LOOPING;
        }
        sound.config.pFilePath = sound.soundPath.c_str();

        //Configure sound buyffer with re-inited config
        ma_result result = ma_sound_init_ex(&SoundEngine::get().m_engine, &sound.config, &sound.soundBuffer);
        if (result != MA_SUCCESS) {
            TraceLog(Dexium::LOG_ERROR, "[Audio]: Failed to init audio from file: {}", std::filesystem::path(sound.soundPath).stem().string());
            fmt::print("MA_Error: {}", ma_result_to_string(result));
            return false;
        }

        sound.firstPlay = false;
        playing = true;
    }
    return true;
}


void AudioStream::play() {
    if (sound.inited) {
        if (sound.firstPlay) {
            // Init the audio
            if (ma_sound_init_ex(&SoundEngine::get().m_engine, &sound.config, &sound.soundBuffer) != MA_SUCCESS) {
                TraceLog(Dexium::LOG_ERROR, "[Audio]: Failed to init audio for file: {}", sound.soundPath);
                return;
            }
            sound.firstPlay = false;
            playing = true;
        }

        if (playing) {
            //Play the audio (Streamed)
            ma_sound_start(&sound.soundBuffer); // Streams on seperate thread
        }
    }
}

void AudioStream::pause() {
    if (playing) {
        if (paused) {
            //Unpause from specified location
            if (ma_sound_seek_to_pcm_frame(&sound.soundBuffer, pausedFrame) != MA_SUCCESS) {
                TraceLog(Dexium::LOG_WARNING, "[Audio]: Failed to capture location of cursor. When resuming the cursor will likely start from the front");
            };
            ma_sound_start(&sound.soundBuffer);
            paused = false;
        } else {
            //Pause track at current location
            ma_sound_get_cursor_in_pcm_frames(&sound.soundBuffer, &pausedFrame);
            ma_sound_stop(&sound.soundBuffer);
            paused = true;
        }
    }
}


void AudioStream::stop() {
    // Stop and free memory
    if (playing) {
        ma_sound_stop(&sound.soundBuffer);
        ma_sound_uninit(&sound.soundBuffer); // Free's the audio buffer memory
        paused = false;
        sound.inited = false;
        sound.firstPlay = true; // Internally flags the sound to be re-read into memory on subsequent play calls

        playing = false;

    }
}


