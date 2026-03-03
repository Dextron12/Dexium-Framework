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


// EnTT header for Signal usage
// REMINDER: EnTT Signals dont take ownership of listeners. Be sure to diconnect at end of lifetime of connected listener
#include <entt/signal/sigh.hpp>


#include <string>

//Forward declare for RenderTarget
namespace Dexium::RenderState {
    class RenderTarget;
}


// This is  SPECIAL namespace that will store ALL global signals
namespace Dexium::Globals::Signals {
    // Remember to disconnect out of scope lsiteners from the signal, or will cause UB
    inline entt::sigh<void(int, int)> sig_onWindowResize; // engine emits signal on window resize
}

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
        int baseWidth, baseHeight; // Stores the inital(design) size of the window

        windowContext(const std::string& windowTitle, int windowWidth, int windowHeight, glfwInitializer* glfwPtr, GLADInitializer* gladPtr);
        ~windowContext();

        // Both functions should eventually be moved to a renderer context
        void startFrame();
        void endFrame();

        RenderState::RenderTarget* getRenderTarget() const;

        // engine internally emits sig_onResize
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    private:
        glfwInitializer* m_glfw_ = nullptr;
        GLADInitializer* m_glad_ = nullptr;

        std::unique_ptr<RenderState::RenderTarget> m_defRenderTarget;

        void onFrameBufferResize(int w, int h);
    };
}


#endif
