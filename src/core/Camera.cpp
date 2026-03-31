//
// Created by Dextron12 on 10/12/25.
//

#include <core/Camera.hpp>
#include <renderer/viewport.hpp>

#include <glm/ext/matrix_clip_space.hpp>

/*
namespace Dexium::Core {

    Camera::Camera(ProjectionType proj) {
        projType = proj;

        // Default init:
        projection = glm::mat4(1.0f);
        windowSize = glm::vec2(0.0f);
    }

    void Camera::setProjection(ProjectionType proj, int scrWidth, int scrHeight, float zNear, float zFar) {
        windowSize = glm::vec2(scrWidth, scrHeight);
        switch (proj) {
            case ProjectionType::ORTHOGRAPHIC:
                // Assumes a 2D projection (render from top to bottom, so that we can render from top left, like typical 2D renderers)
                // Renders in (px)
                projection = glm::ortho(0.0f, windowSize.x, windowSize.y, 0.0f); // Forces a top to bottom, left to right renderer
                break;
            case ProjectionType::PERSPECTIVE:
                // Assumes  a 3D projection (left to right, bottom to top)
                projection = glm::perspective(glm::radians(45.0f), (float)windowSize.x / (float)windowSize.y, zNear, zFar);
        }
    }

    const glm::mat4& Camera::getProjection() const {
        return projection;
    }


}
*/

namespace Dexium::Core {

    const glm::mat4& baseCamera::getViewMatrix() {
        return viewMatrix;
    }

    Camera2D::Camera2D() {
        //Default construct transform attribs, so View doesnt produce garbage on initalisation
        transform.position = glm::vec3(0.f);
        transform.scale = glm::vec3(1.f); // IMPORTANT!!
        transform.rotation = glm::vec3(0.f);
    }

    void Camera2D::update() {
        // Update View from Transform data
        // View matrix is the inverse of the camera positon!!
        viewMatrix = glm::inverse(transform.ModelMatrix());

    }

    const glm::mat4& Camera2D::getProjectionMatrix(const Renderer::Viewport& vp) {
        // Renderer ultimatly decides the viewport/aspectRatio, so camera jsut takes it and provides its own projection
        //return glm::ortho()

        // Check if Projection matrix needs to be re-calculated
        if (vp != cachedVp) {
            // Calculate new projection matrix:
            projectionMatrix = glm::ortho(static_cast<float>(vp.x), static_cast<float>(vp.w), static_cast<float>(vp.h), static_cast<float>(vp.y), -1.f, 1.f);
            cachedVp = vp;
        }

        return projectionMatrix;


    }
}
