//
// Created by Dextron12 on 7/3/26.
//

#ifndef DEXIUM_WINDOWCONTEXT_HPP
#define DEXIUM_WINDOWCONTEXT_HPP

#include <string_view>
#include <memory>

#include <core/BitwiseFlag.hpp>
#include <core/Signal.hpp>

#include <glm/vec2.hpp>

//Forward declartion for GLFWwindow
struct GLFWwindow;

//Forward declare for RenderTarget
namespace Dexium::RenderState {
    class RenderTarget;
}

namespace Dexium::Core {
    class Input; // Forward declare for Core::Input
}

//Storage for engine elvel signals
namespace Dexium::Private::Signals {
    extern Core::Signal<int, int> Sig_OnWinResize;
}


namespace Dexium::Initializers {
    class glfwInitializer {
        public:
        glfwInitializer();
        ~glfwInitializer();

        [[nodiscard]] bool isValid() const;

    private:
        void init();
        bool m_inited = false;
    };

    class gladInitializer {
    public:
        gladInitializer();
        ~gladInitializer();

        [[nodiscard]] bool isValid() const;

    private:
        void init();
        bool m_inited = false;
    };
}


namespace Dexium::Core {

    enum class WindowHints {
        Default = 0,
        Resizable = 1 << 0,
        Visible = 1 << 1,
        Decorated = 1 << 2,
        Focused = 1 << 3
    };
    template <>
    struct EnableBitmaskOperators<WindowHints> {
        static constexpr bool value = true;
    };

    struct GLContextVersion {
        explicit GLContextVersion(int _major = 3, int _minor = 3)
            : major(_major), minor(_minor)  {}
        int major;
        int minor;

    };

    class WindowContext {
        public:
        WindowContext(std::string_view windowName, int windowWidth, int windowHeight, WindowHints winFlags = WindowHints::Default, GLContextVersion glContextVersion = GLContextVersion{});
        ~WindowContext();

        [[nodiscard]] glm::ivec2 getCurrentSize() const;
        [[nodiscard]] glm::ivec2 getDesignSize() const;

        GLFWwindow* getWindow() const;

        RenderState::RenderTarget& getRenderTarget();

        bool shouldClose();

        Input& getInput() const;

        void swapBuffers(); // Swaps the window buffers
        void pollEvents(); // Polls GLFW event state

        bool windowResized();

        Input& getInput();

    private:
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        //Sets the required state for the new window
        void setWindowFlags(WindowHints winFlags, GLContextVersion ver);

        // Initalisers:
        //std::unique_ptr<Initializers::glfwInitializer> _glfwInit; // The EngineState should really own this and ensure its inited before window creation
        std::unique_ptr<Initializers::gladInitializer> _gladInit;

        GLFWwindow* m_window = nullptr;
        glm::ivec2 m_designSize;
        glm::ivec2 m_windowSize;
        glm::ivec2 m_windowSize_old;

        // Input system (per window)
        std::unique_ptr<Input> m_inputSys;
        //Default renderTarget(Sized to full window)
        std::unique_ptr<RenderState::RenderTarget> m_defRenderTarget;
    };
}

#endif //DEXIUM_WINDOWCONTEXT_HPP