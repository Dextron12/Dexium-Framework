//
// Created by Dextron12 on 10/09/2025.
//

#include <core/helpers.h>

#include <glad/glad.h>
#include <glm/vec4.hpp>

#include <string>
#include <iostream>

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

std::shared_ptr<Dexium::Layer> Dexium::Helpers::CreateLayer(const std::string &layerID, std::function<void()> onInit, std::function<void()> onUpdate, std::function<void()> onRender) {
    auto layer = std::make_shared<Dexium::Layer>(layerID);

    layer->onInit = onInit;
    layer->onUpdate = onUpdate;
    layer->onRender = onRender;

    return layer;
}

