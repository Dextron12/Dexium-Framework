//
// Created by ethan on 7/12/25.
//

#ifndef DEXIUM_TRANSFORM_H
#define DEXIUM_TRANSFORM_H

#include <glm/glm.hpp>

namespace Dexium::Core {

    class Transform {
    public:
        glm::vec3 position;
        glm::vec3 rotation; // Rot is done in euler angles (radians) -> (pitch, yaw, roll)
        glm::vec3 scale;

        Transform() = default;
        Transform(glm::vec3 pos, glm::vec3 rot = {0.f, 0.f, 0.f}, glm::vec3 scale = {1.f, 1.f, 1.f})
            :position(pos), rotation(rot), scale(scale) {}

        // Returns a glm::mat4 model matrix of this transform(pos + rot + scale)
        glm::mat4 ModelMatrix();
    };


}


#endif //DEXIUM_TRANSFORM_H