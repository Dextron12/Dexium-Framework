//
// Created by Dextron12 on 1/12/25.
//

#include <iostream>

#include <core/windowContext.hpp>

namespace Dexium::Core {

    windowContext::windowContext(const std::string &windowTitle, int windowWidth, int windowHeight, glfwInitializer* glfwPtr, GLADInitializer* gladPtr) : m_glfw_(glfwPtr), m_glad_(gladPtr) {

        // Lazy init GLFw (if needed)
        if (!m_glfw_ || !m_glfw_->isValid()) {
            if (!glfwPtr) {
                std::cout << "No valid GLFWInitialiser provided" << std::endl;
                return;
            } else {m_glfw_->init();}
        }

        // Create window/context
        window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
        if (!window) {
            std::cout << "Failed to create GLFW window" << std::endl;
            return;
        }
        glfwMakeContextCurrent(window);

        //Init GLAD
        if (!gladLoaderLoadGL()) {
            std::cout << "Failed to load GLAD" << std::endl;
            return;
        }

        width = windowWidth;
        height = windowHeight;
    }

    windowContext::~windowContext() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }


}