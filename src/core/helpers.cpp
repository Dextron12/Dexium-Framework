//
// Created by Dextron12 on 10/09/2025.
//

#include <core/helpers.hpp>

#include <glm/vec4.hpp>

#include <string>
#include <iostream>

#include <glad/glad.h>

#include "core/Texture.hpp"
#include "fmt/compile.h"

void clearScreen(float r, float g, float b, float a, GLbitfield buffer_mask) {
    float n_r = r / 255.0f;
    float n_g = g / 255.0f;
    float n_b = b / 255.0f;
    float n_a = a / 255.0f;

    // In case I foroget prefix(N_) = normalised_*
    glClearColor(n_r, n_g, n_b, n_a);
    glClear(buffer_mask);
}

void clearScreen(glm::vec4 colour, GLbitfield buffer_mask) {
    clearScreen(colour[0], colour[1], colour[2], colour[3], buffer_mask);
}

void checkGLErrors(const std::string& label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[OpenGL Error]";
        if (!label.empty()) std::cerr << " [" << label << "]";
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << ": GL_INVALID_ENUM\n"; break;
            case GL_INVALID_VALUE:
                std::cerr << ": GL_INVALID_VALUE\n"; break;
            case GL_INVALID_OPERATION:
                std::cerr << ": GL_INVALID_OPERATION\n"; break;
            case GL_STACK_OVERFLOW:
                std::cerr << ": GL_STACK_OVERFLOW\n"; break;
            case GL_STACK_UNDERFLOW:
                std::cerr << ": GL_STACK_UNDERFLOW\n"; break;
            case GL_OUT_OF_MEMORY:
                std::cerr << ": GL_OUT_OF_MEMORY\n"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << ": GL_INVALID_FRAMEBUFFER_OPERATION\n"; break;
            default:
                std::cerr << ": Unknown error (0x" << std::hex << err << ")\n";
        }
    }
}


void Dexium::Utilities::registerWhiteTexture() {
    Texture2D tex;
    glGenTextures(1, &tex.ID);
    glActiveTexture(GL_TEXTURE0); // Used jsut for creating the texture
    glBindTexture(GL_TEXTURE_2D, tex.ID);

    unsigned char whitePixel[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0); // uNBIND THE CURRENT TEXTURE TO PREVENT MODIFICATION

    //register texture
    auto& assets = AssetManager::getInstance();
    assets.registerAsset("__whiteTexture", tex);

}


void Dexium::Utilities::registerFallbackTexture(int rows, int cols, int cellSize, std::string_view id) {
    //cellSize// pixels per checker cell (adjsut for bigger blocks)
    const int width = cols * cellSize;
    const int height = rows * cellSize;

    // Create a pixel buffer (RGBA, 4 BYTES PER PIXEL)
    std::vector<unsigned char> pixels(width * height * 4);

    //Define two colours 9Swap for Colour when implemented!)
    const unsigned char black[4] = {0, 0, 0, 255};
    const unsigned char purple[4] = {128, 0, 128, 255}; // (255,0,255,255) for bright magenta

    // Fill in the checkerboard apttern
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool usePurple = ((x / cellSize) + (y / cellSize)) % 2 == 0;
            const unsigned char* color = usePurple ? purple : black;

            int index = (y * width + x) * 4;
            pixels[index + 0] = color[0];
            pixels[index + 1] = color[1];
            pixels[index + 2] = color[2];
            pixels[index + 3] = color[3];
        }
    }

    // Create the texture
    Texture2D tex;
    glGenTextures(1, &tex.ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.ID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    //Register in asset manager
    auto& assets = AssetManager::getInstance();
    assets.registerAsset(id.data(), tex);


}

