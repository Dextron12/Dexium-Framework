//
// Created by Dextron12 on 10/12/25.
//

#include <core/Camera.hpp>
#include <glm/ext/matrix_clip_space.hpp>

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
