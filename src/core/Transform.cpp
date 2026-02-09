//
// Created by ethan on 7/12/25.
//

#include <core/Transform.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Dexium::Core {

    glm::mat4 Transform::ModelMatrix() {
        //The model
        glm::mat4 model(1.0f);

        // Some tranform boundry checks were needed to mitigate bug #04
        // but they simply just output warnings if the model was too small or alrge for the viewport/camera
        // In fuuture, we could reimplement this, but from here on we assume its the end-users problem if they render something outside of the viewport

        // Translation
        model = glm::translate(model, position);

        // Rotation -> In the ZYX order (common for graphics engines)
        model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));
        model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
        model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));

        // Scale
        model = glm::scale(model, scale);

        return model;
    }


}

