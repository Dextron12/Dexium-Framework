//
// Created by Dextron12 on 10/09/2025.
//

#ifndef DEXIUM_HELPERS_H
#define DEXIUM_HELPERS_H
#include <string>

#include "Layers.hpp"

/* -- DEPRECATED FUNCTIONS -> wILL BE REPLACED WITH Colour -- */
// Provide RGBA coords from 0 -> 255 (Internally converts to [0,1] GL)
//void clearScreen(float r, float g, float b, float a, GLbitfield buffer_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//void clearScreen(glm::vec4 colour, GLbitfield buffer_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

// HELPFUL : Checks the GL state for any internal error messages and tries to give a less confusing error message!
void checkGLErrors(const std::string& label = "");


namespace Dexium {

    namespace Utilities {
        // Registers a fallback texture (pink & black checkerboard) to the AssetManager as '__fallbackTexture'
        void registerFallbackTexture(int rows, int cols, int cellSize = 8, std::string_view assetID = "__fallbackTexture");

        // Registers the default 1x1 white texture required for all rendering
        void registerWhiteTexture();
    }
}


#endif //DEXIUM_HELPERS_H