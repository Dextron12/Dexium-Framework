//
// Created by Dextron12 on 10/09/2025.
//

#ifndef DEXIUM_HELPERS_H
#define DEXIUM_HELPERS_H
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <string>

#include "Layers.hpp"

/* -- DEPRECATED FUNCTIONS -> wILL BE REPLACED WITH Colour -- */
// Provide RGBA coords from 0 -> 255 (Internally converts to [0,1] GL)
//void clearScreen(float r, float g, float b, float a, GLbitfield buffer_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//void clearScreen(glm::vec4 colour, GLbitfield buffer_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

// HELPFUL : Checks the GL state for any internal error messages and tries to give a less confusing error message!
void checkGLErrors(const std::string& label = "");

namespace Dexium::Helpers {
    std::shared_ptr<Layer> CreateLayer(
        const std::string& layerID,
        std::function<void()> onInit = nullptr,
        std::function<void()> onUpdate = nullptr,
        std::function<void()> onRender = nullptr
        );
}

#endif //DEXIUM_HELPERS_H