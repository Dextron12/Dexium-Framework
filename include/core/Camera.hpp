//
// Created by Dextron12 on 03/03/26.
//

#ifndef DEXIUM_CAMERA_HPP
#define DEXIUM_CAMERA_HPP


#include <core/Transform.h>

#include <glm/glm.hpp>

namespace Dexium::RenderState {
    class Viewport;
}

namespace Dexium::Core {

    class baseCamera {
    public:
        baseCamera() : viewMatrix(glm::mat4(1.0f)), projectionMatrix(glm::mat4(1.0f)) {}

        const glm::mat4& getViewMatrix();
        const glm::mat4& getProjectionMatrix();

        virtual ~baseCamera() = default;

        Transform transform;

    protected:
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

    };

    class Camera2D : public baseCamera {
        public:
            Camera2D(RenderState::Viewport* viewport = nullptr);

            void update(RenderState::Viewport* viewport); // Updates the View matrix from the transform data
    };

}

#endif
