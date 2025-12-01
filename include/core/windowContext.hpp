//
// Created by Dextron12 on 1/12/25.
//

#ifndef DEXIUM_WINDOW_HPP
#define DEXIUM_WINDOW_HPP
#include <iostream>
#include <ostream>

#ifdef _WIN32
#include <Windows.h> // LEAN_AND_MEAN should be used!!
#endif

//OpenGL headers
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>

namespace Dexium::Core {

    //RAII wrapper for glfwContext
    class glfwInitializer {
    public:
        glfwInitializer() {
            init();
        }

        ~glfwInitializer() {
            glfwTerminate();
        }

        void init() {
            if (!glfwInit()) {
                std::cout << "Failed to initialize GLFW" << std::endl;
                m_inited = false;
            } else {
                m_inited = true;
            }
        }

        bool isValid() const { return m_inited; }

    private:
        bool m_inited = false;
    };

    // RAII gladInializer
    class GLADInitializer {
    public:
        GLADInitializer() {
            if (!gladLoaderLoadGL()) {
                std::cout << "Failed to initialize GLAD" << std::endl;
                m_inited = false;
            } else { m_inited = true; }
        }

        ~GLADInitializer() {
            gladLoaderUnloadGL();
        }

        bool isValid() const { return m_inited; }

    private:
        bool m_inited = false;
    };

    class windowContext {
    public:
        GLFWwindow* window;
        int width, height;

        windowContext(const std::string& windowTitle, int windowWidth, int windowHeight, glfwInitializer* glfwPtr, GLADInitializer* gladPtr);
        ~windowContext();

        // Both functions should eventually be moved to a renderer context
        void startFrame();
        void endFrame();

        // Provide a Signal for window Resize here:

    private:
        glfwInitializer* m_glfw_ = nullptr;
        GLADInitializer* m_glad_ = nullptr;
    };
}


#endif
