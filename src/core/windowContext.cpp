//
// Created by Dextron12 on 7/3/26.
//

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <core/windowContext.hpp>
#include <core/Input.hpp>
#include <renderer/RenderTarget.hpp>
#include <core/Error.hpp>

#include <stdexcept> // Used to throw runtime_error on core library failure

#include "core/Controller.hpp"

namespace Dexium::Initializers {

    glfwInitializer::glfwInitializer() {
        init();
    }

    glfwInitializer::~glfwInitializer() {
        glfwTerminate();
    }

    void glfwInitializer::init() {
        if (m_inited) return;

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        m_inited = true;
    }

    bool glfwInitializer::isValid() const {
        return m_inited;
    }

    gladInitializer::gladInitializer() {
        init();
    }

    gladInitializer::~gladInitializer() {
        gladLoaderUnloadGL();
    }

    void gladInitializer::init() {
        if (m_inited) return;

        if (!gladLoaderLoadGL()) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        m_inited = true;
    }

    bool gladInitializer::isValid() const {
        return m_inited;
    }
}

namespace Dexium::Private::Signals {
    Core::Signal<int, int> Sig_OnWinResize;
}


namespace Dexium::Core {
    void WindowContext::setWindowFlags(WindowHints winFlags, GLContextVersion ver) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver.minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //glfwWindowHint(GLFW_)
    }

    WindowContext::WindowContext(std::string_view windowName, int windowWidth, int windowHeight, WindowHints winFlags, GLContextVersion glContextVersion) {
        setWindowFlags(winFlags, glContextVersion);

        //Create window/conext
        m_window = glfwCreateWindow(windowWidth, windowHeight, windowName.data(), nullptr, nullptr);
        if (!m_window) {
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(m_window);

        // Init GLAD
        _gladInit = std::make_unique<Initializers::gladInitializer>();

        //COnfgiure user windowc allback
        glfwSetWindowUserPointer(m_window, this);

        m_designSize = glm::ivec2(windowWidth, windowHeight);
        m_windowSize = {windowWidth, windowHeight};
        m_windowSize_old = m_windowSize;

        //Create defauklt renderTarget(Targets the entire window)
        m_defRenderTarget = std::make_unique<RenderState::RenderTarget>(
            RenderState::Viewport{0,0,m_windowSize.x,m_windowSize.y}
            );

        //Init the window wide input system
        m_inputSys = std::make_unique<Input>();

        //Set window callbacks
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetKeyCallback(m_window, key_callback);
    }

    WindowContext::~WindowContext() {
        _gladInit = nullptr; // Deinits glad
        glfwDestroyWindow(m_window);
    }

    glm::ivec2 WindowContext::getDesignSize() const {
        return m_designSize;
    }

    glm::ivec2 WindowContext::getCurrentSize() const {
        return m_windowSize;
    }

    GLFWwindow* WindowContext::getWindow() const {
        return m_window;
    }

    RenderState::RenderTarget& WindowContext::getRenderTarget() {
        return *m_defRenderTarget;
    }

    Input& WindowContext::getInput() const {
        return *m_inputSys;
    }

    bool WindowContext::shouldClose() {
        return glfwWindowShouldClose(m_window);
    }

    void WindowContext::swapBuffers() {
        glfwSwapBuffers(m_window);
    }

    void WindowContext::pollEvents() {
        glfwPollEvents();
    }

    Input& WindowContext::getInput() {
        return *m_inputSys;
    }

    void WindowContext::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        // Emit the onResize sing
        Private::Signals::Sig_OnWinResize.emit(width, height);

        // Access the Window context and modify
        auto* ctx = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
        if (!ctx) {
            TraceLog(LogLevel::WARNING, "[WindowContext]: Failed to fetch windowPtr on even resize callback. Window size won't update automatically");
            return;
        }

        //Update window ctx
        ctx->m_windowSize_old = ctx->m_windowSize;
        ctx->m_windowSize.x = width;
        ctx->m_windowSize.y = height;

        // Update default renderTarget
        ctx->m_defRenderTarget->m_viewport.w = width;
        ctx->m_defRenderTarget->m_viewport.h = height;
        // IMPORTANT: The renderer now soley calls glViewport and will see that the viewport size has chanegd and make a call to glViewport, so its not needed here
    }

    void WindowContext::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == 0) return;

        auto* ctx = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
        auto&[m_currentKey, m_prevKey] = ctx->getInput();

        if (action == GLFW_PRESS) {
            m_currentKey[key] = true;
        } else if (action == GLFW_RELEASE) {
            m_currentKey[key] = false;
        }
    }

    bool WindowContext::windowResized() {
        return ( (m_windowSize.x != m_windowSize_old.x) || (m_windowSize.y != m_windowSize_old.y) );
    }







}