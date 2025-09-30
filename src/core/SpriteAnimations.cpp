//
// Created by Dextron12 on 22/09/2025.
//

#include "SpriteAnimations.hpp"

#ifdef DEXIUM_HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#include <config.hpp> // Contains engine_core versioning & core configurations
#include <fstream> // For file writing

#endif


AnimatedSprite::AnimatedSprite(const std::string &fileName, std::shared_ptr<Camera> camera) {
    m_spr = std::make_unique<Spritesheet>(fileName);
    if (!m_spr) {
        TraceLog(LOG_ERROR, "[SpriteAnimator]: Failed to generate Sprite obj");
        return;
    }

    if (!camera) {
        TraceLog(LOG_WARNING, "[SpriteAnimator]: No camera specified!");
        return;
    }

    m_spr->camera = camera;
}

void AnimatedSprite::setShader(const std::string& shaderID) {
    m_spr->setShader(shaderID);
}

void AnimatedSprite::setSequence(const std::string& name, std::initializer_list<newSeqFrame> frames, float defaultDuration, bool loop) {
    if (!m_spr) {
        TraceLog(LOG_ERROR, "[SpriteAnimator]: This is a corrupt animator. Please retire this object!");
        return;
    }

    // Cech if a sequence of the same name already exists:
    auto it = m_sequences.find(name);
    if (it != m_sequences.end()) {
        TraceLog(LOG_ERROR, "[SpriteAnimator]: The sequence '%s' is already defined", name.c_str());
        return;
    }

    // Create new sequence and sprite data
    animatedSequence seq;
    seq.timeMS = defaultDuration; // Set defaultDuration

    // Iterate through frames, construct metadata and allow Spreitesheet tor egister UVs
    for (const auto& f : frames) {
        seq.frames.push_back({f.id, {f.id, f.duration}});

        // Set UV data for Spritesheet
        m_spr->setSprite(f.id, f.uvRect); // Uses Spritesheet to internally hold the subTexture UV data
    }

    seq.loop = loop; // Configure the state loop flag
    m_sequences[name] = std::move(seq);

}

void AnimatedSprite::popSequence(const std::string &name) {
    auto seq = m_sequences.find(name);
    if (seq != m_sequences.end()) {
        m_sequences.erase(seq);
    }
}

void AnimatedSprite::setFrameTime(const std::string &subSpriteID, float timeMS) {
    // Iterate through sequences looking for any matches
    for (auto& seq : m_sequences) {
        auto it = std::find_if(seq.second.frames.begin(), seq.second.frames.end(),
            [&](auto& f){return f.first == subSpriteID;});

        if (it != seq.second.frames.end()) {
            // Modify the frame time
            it->second.duration = timeMS;
            return; // Stop searching, No more than 1 frameID should exist, anymore implies duplicates of the same frame
        }
    }

    TraceLog(LOG_WARNING, "[SpriteAnimator]: FrameID '%s' couldn't be found in the current sequences.\nCould not adjust frame timing to '%f'", subSpriteID.c_str(), timeMS);
}

void AnimatedSprite::resetFrameTime(const std::string &subSpriteID) {
    setFrameTime(subSpriteID, 0.f);
}

bool AnimatedSprite::hasFrameChanged() {
    return m_changedFrame;
}


void AnimatedSprite::changeDefaultTime(const std::string &seqID, float timeMS) {
    // Find seq
    auto seq = m_sequences.find(seqID);
    if (seq != m_sequences.end()) {
        //Modify default sequence value
        seq->second.timeMS = timeMS;
    }
}

void AnimatedSprite::update(const std::string &seqID, float deltaTime) {
    auto it = m_sequences.find(seqID);
    if (it == m_sequences.end()) {
        TraceLog(LOG_ERROR, "[SpriteAnimator]: Cannot update sequence: '%s', it does not exist", seqID.c_str());
        return;
    }

    auto& seq = it->second;

    if (m_currentSequence != seqID) {
        // Anim sequence has changed, reset animation:
        m_currentFrame = 0;
        m_frametimer = 0.f;
        m_currentSequence = seqID; // Reset complete
    }

    const auto& frames = seq.frames[m_currentFrame];

    // decide wheter to use sequence default time or pre-defined frame time
    float frameTime = (frames.second.duration == 0.0f) ? seq.timeMS : frames.second.duration;
    m_frametimer += deltaTime; // Increment timer

    // Check timer state
    if (m_frametimer >= frameTime) {
        m_frametimer = 0.f;
        m_currentFrame++;
        m_changedFrame = true;

        //Check for end of sequence/loop
        if (m_currentFrame >= seq.frames.size()) {
            if (seq.loop) {
                m_currentFrame = 0; // Loop sequence
            } else {
                m_currentFrame = seq.frames.size() - 1; // Stay on last frame
                m_endSequence = true;
            }
        }
    } else {
        if (m_changedFrame) m_changedFrame = false; // Allows changedFrame toa ct more as a signal
    }
}

void AnimatedSprite::render(const std::string& seqName, const glm::vec4& pos) {
    // Assumes the end-user has called update(seqID) for the requested sequence

    //Check if the sequence exists:
    if (m_sequences.find(seqName) == m_sequences.end()) {
        //const auto& it = m_sequences[seqName];
        TraceLog(LOG_ERROR, "[SpriteAnimator]: Cannot render '%s', the sequence does not exist");
        return;
    }
    auto& seq = m_sequences[seqName]; // Sequence
    const auto& idx = seq.frames[m_currentFrame]; // Current frame obj ref(const)


    //draw sprite
    m_spr->drawSprite(idx.second.id, {pos.x, pos.y, 0.0f}, {pos.z, pos.w, 0.0f}, glm::vec3(0));
}

void AnimatedSprite::updateAndRender(const std::string &seqName, float deltaTime, const glm::vec4 &pos) {
    update(seqName, deltaTime);
    render(seqName, pos);
}


void AnimatedSprite::writeToFile(const std::string& fileName) {
    // Check for any sequences to write
    if (m_sequences.empty()) {
        TraceLog(LOG_INFO, "[SpriteAnimator]: No sequences to record to file");
        return;
    }

    // Check if Dexium-Core is built with the optional nlohmann-json lib
    // nlohmann_json is required for this function
#ifndef DEXIUM_HAS_NLOHMANN_JSON
    TraceLog(LOG_ERROR, "[SpriteAnimator_WriterJSON]: Current Dexium_Core was not built with optional nlohmann_json lib. This feature requires that lib");
    return;
#else

    nlohmann::ordered_json j;

    // Write Header
    j["Version"]["engine"] = "dexium-core";
    j["Version"]["major"] = VERSION::major;
    j["Version"]["minor"] = VERSION::minor;
    j["Version"]["patch"] = VERSION::patch;
    j["Version"]["uuid"] = m_spr->getTextureID(); // Sets the uuid to the texture asset ID.


    TraceLog(LOG_INFO, "[SpriteAnimator_Writer]: Writing %i sequences to file", m_sequences.size());

    std::cout << j << std::endl;

    const auto& uvMap = m_spr->getSpriteFrames();

    // Write sequences
    for (const auto& seq : m_sequences) {
        nlohmann::ordered_json& frames = j["Sequences"][seq.first];
        if (!frames.is_array()) {
            // Force nlohmann to use it as an array
            frames = nlohmann::ordered_json::array();
        }

        for (const auto& fr : seq.second.frames) {

            // Get duration
            float duration = (seq.second.timeMS == 0.0f) ? seq.second.timeMS : fr.second.duration;

            // Submit data
            //Sequence: [frame1: duration, frame2: duration...]

            const glm::vec4& uv = uvMap.find(fr.first)->second;

            // Push a frame object into the sequence array
            frames.push_back({
            {"frameID", fr.first},
            {"duration", duration},
            {"uv", {uv.x, uv.y, uv.z, uv.w}}
            });

        }
    }

    // TEST: OUPUTS THE FULL JSON FILE TO TERMINAL
    //std::cout << j.dump(4) << std::endl;

    // Output to file
    std::ofstream file(fileName + ".json"); // Adds the json file extension to provided name, regardless of provided file extension
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "[SpriteAnimator_Writer]: Failed to write data to '%s.json'\nThe file could not be opened", fileName.c_str());
        return;
    }

    //prety-print with 4 spaces
    file << j.dump(4);
    file.close();
    TraceLog(LOG_INFO, "[SpriteAnimator-Writer]: Successfully written current animation state to '%s.json'", fileName.c_str());


#endif



}