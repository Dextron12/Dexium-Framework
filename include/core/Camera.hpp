//
// Created by Dextron12 on 03/03/26.
//

#ifndef DEXIUM_CAMERA_HPP
#define DEXIUM_CAMERA_HPP


#include <core/Transform.h>

#include <glm/glm.hpp>

namespace Dexium::Core {

    class baseCamera : public Transform {
    public:
        baseCamera() : viewMatrix(glm::mat4(1.0f)), projectionMatrix(glm::mat4(1.0f)) {}

        virtual const glm::mat4& getViewMatrix();
        virtual const glm::mat4& getProjectionMatrix();

        virtual ~baseCamera();

    private:
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

    };

}

#endif
