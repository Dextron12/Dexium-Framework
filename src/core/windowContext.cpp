//
// Created by Dextron12 on 1/12/25.
//

#include <iostream>

#include <core/windowContext.hpp>

#include <renderer/RenderTarget.hpp>
#include <Dexium.hpp>


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

        // Set glfw window resize callback
        // Callback triggers the sig
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, windowContext::framebuffer_size_callback);

        width = windowWidth; height = windowHeight;
        baseWidth = windowWidth; baseHeight = windowHeight;

        //Create default renderTarget
        m_defRenderTarget = std::make_unique<RenderState::RenderTarget>(
            RenderState::Viewport{0, 0, width, height}
        );
    }

    windowContext::~windowContext() {
        // Manually destroung the window with glfwDestroyWindow() results in an internal
        // double free of glState and causes glTerminate() to throw a segfault
    }

    void windowContext::startFrame() {
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            EngineState::get().m_appState = false;
        }
    }

    void windowContext::endFrame() {
        glfwSwapBuffers(window);
    }

    void windowContext::framebuffer_size_callback(GLFWwindow *win, int w, int h) {
        //capture the context of the window
        auto* ctx = static_cast<windowContext*>(
            glfwGetWindowUserPointer(win));
        if (ctx) {
            ctx->onFrameBufferResize(w, h);
        }
    }

    void windowContext::onFrameBufferResize(int w, int h) {
        Globals::Signals::sig_onWindowResize.publish(w, h);

        //TraceLog(LogLevel::STATUS, "[Window]: Context has resized from {}x{} to {}x{}", width, height, w, h);

        // Update w & h
        width = w; height = h;

        // Update default viewport
        m_defRenderTarget->m_viewport.w = width;
        m_defRenderTarget->m_viewport.h = height;
    }

    RenderState::RenderTarget* windowContext::getRenderTarget() const {
        return m_defRenderTarget.get();
    }



}
