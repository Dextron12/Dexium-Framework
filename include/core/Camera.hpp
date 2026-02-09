//
// Created by Dextron12 on 10/12/25.
//

#ifndef DEXIUM_CAMERA_H
#define DEXIUM_CAMERA_H

#include <glm/glm.hpp>

namespace Dexium::Core {

    enum class ProjectionType {
        PERSPECTIVE, // 3D
        ORTHOGRAPHIC, // 2D
    };

    class Camera {
    public:
        Camera(ProjectionType projection);

        void setProjection(ProjectionType proj, int scrWidth, int scrHeight, float zNear = 0.1f, float zFar = 50.0f);

        const glm::mat4& getProjection() const;

        ProjectionType projType;

    private:
        glm::mat4 projection;
        glm::vec2 windowSize;
    };
}


#endif //DEXIUM_CAMERA_H