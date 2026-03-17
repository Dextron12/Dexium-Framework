//
// Created by Dextron12 on 10/12/25.
//

#include <core/Camera.hpp>
#include <renderer/viewport.hpp>

#include <Dexium.hpp>

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
    const glm::mat4& baseCamera::getProjectionMatrix() {
        return projectionMatrix;
    }

    const glm::mat4& baseCamera::getViewMatrix() {
        return viewMatrix;
    }

    Camera2D::Camera2D(RenderState::Viewport* viewport) {
        //Default construct transform attribs, so View doesnt produce garbage on initalisation
        transform.position = glm::vec3(0.f);
        transform.scale = glm::vec3(1.f); // IMPORTANT!!
        transform.rotation = glm::vec3(0.f);
        update(viewport);
    }

    void Camera2D::update(RenderState::Viewport* viewport) {
        // Check if a Projection amtrix is valid || needs updating?

        // If we are modifiying proj in any way before initalisation or even at, consider using glm::epsilonEqual for comparision (to look for an identity amtrix)
        if (projectionMatrix == glm::mat4(1.0f) && viewport == nullptr) {
            // Need to generate a projeciton absed of the default viewport
            viewport = EngineState::get().getDefaultViewport();

        }
        // Now we can safely assume that if viewport points to an address, the projection needs updating
        if (viewport) {
            // Update projection matrix:
            // A Simple 2D/Ortho projection in the range of (vp.x -> vp.w, vp.y -> vp.h) creating a top-left centric projection, whciht he Renderer expects for 2D
            projectionMatrix = glm::ortho(static_cast<float>(viewport->x), static_cast<float>(viewport->w), static_cast<float>(viewport->y), static_cast<float>(viewport->h), 0.1f, 1.f);
        }

        // Update View from Transform data
        // View matrix is the inverse of the camera positon!!

        viewMatrix = glm::inverse(transform.ModelMatrix());

    }
}
