//
// Created by Dextron12 on 03/03/26.
//

#ifndef DEXIUM_CAMERA_HPP
#define DEXIUM_CAMERA_HPP


#include <core/Transform.h>
#include <renderer/viewport.hpp>

#include <glm/glm.hpp>


namespace Dexium::Core {

    class baseCamera {
    public:
        baseCamera() : viewMatrix(glm::mat4(1.0f)), projectionMatrix(glm::mat4(1.0f)) {}

        const glm::mat4& getViewMatrix();
        virtual const glm::mat4& getProjectionMatrix(const Renderer::Viewport& vp) = 0;

        virtual ~baseCamera() = default;

        virtual void update() = 0;

        Transform transform;

    protected:
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        // Store a ciopy of the last viewport used to compute the proj mat
        Renderer::Viewport cachedVp = {};
    };

    class Camera2D final : public baseCamera {
        public:
            Camera2D();

            const glm::mat4& getProjectionMatrix(const Renderer::Viewport& vp) override;

            void update() override; // Updates the View matrix from the transform data
    };

}

#endif
