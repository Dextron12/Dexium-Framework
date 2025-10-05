//
// Created by Dextron12 on 22/09/2025.
//

#ifndef DEXIUM_SPRITEANIMATIONS_HPP
#define DEXIUM_SPRITEANIMATIONS_HPP


#include <string>
#include <vector>
#include <memory>

#include <core/Sprite.hpp>

namespace Dexium {


    // Only used for sequence creation. internal Spritesheet obj holds UV data
    struct newSeqFrame {
        std::string id;
        glm::vec4 uvRect;
        float duration; // in ms

        newSeqFrame(const std::string& id_, const glm::vec4& uvRect_, float duration_ = 0.0f, bool loop_=true)
            : id(id_), uvRect(uvRect_), duration(duration_) {}
    };

    struct Frame {
        std::string id; // subTexture ID
        float duration;

        Frame(std::string id_, float duration_ = 0.0f) : id(id_), duration(duration_) {}
    };

    struct animatedSequence {
        //std::unordered_map<std::string, size_t> frameIndex;
        //std::vector<Frame> frames;
        std::vector<std::pair<std::string, Frame>> frames;

        float timeMS; // Time used for the entire sequence (overrrides frame timer)

        bool loop;
    };

    // Single layer animated sprite
    class AnimatedSprite {
    public:
        AnimatedSprite(const std::string& fileName, std::shared_ptr<Camera> camera = std::shared_ptr<Camera>{});

        void setSequence(const std::string& name, std::initializer_list<newSeqFrame> frames, float defaultDuration, bool loop = true);
        void popSequence(const std::string& name);

        // Sets the per-frame timer (specify a subTextureID for lookup)
        void setFrameTime(const std::string& subSpriteID, float timeMS);
        void resetFrameTime(const std::string& subSpriteID); // sets the provided frame to the sequence time(defaultDuration)

        void changeDefaultTime(const std::string& seqID, float timeMS);

        // Sets the internal shader for Uniform configuration
        void setShader(const std::string& shaderID);

        /* AutoSliceByRow() takes each row and slices it into sequences
        - padding: the margin around the atlas
        - spacing: the spacing between sprites (tries to deduce the sub texture size from this)
         */
        void AutoSliceByRow(const glm::vec2& padding, const glm::vec2 spacing,
            const std::vector<std::string>& frameNames, float defaultTimeMS);

        void readSequencesFromJSON(const std::string& fileName);
        void writeToFile(const std::string& fileName); // Writes ALL configured sequences to file

        // Returns true for whenever a change of frame occurs. Use this to implement custom detail on each frame
        bool hasFrameChanged(); // Needs trackers for state change


        void update(const std::string& seqID, float deltaTime);
        void render(const std::string& seqName, const glm::vec4& pos); // Renders at pos + scale(z,w). Just use z&y if you dont wish to scale
        void updateAndRender(const std::string& seqName, float deltaTime, const glm::vec4& pos); // Convience function

    private:
        std::unique_ptr<Spritesheet> m_spr;

        std::unordered_map<std::string, animatedSequence> m_sequences;

        std::string m_currentSequence;
        size_t m_currentFrame = 0;
        float m_frametimer = 0.0f;

        bool m_changedFrame = false;
        bool m_endSequence = false;

    };



}

#endif //DEXIUM_SPRITEANIMATIONS_HPP