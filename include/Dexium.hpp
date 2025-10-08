// DEXIUM-FRAMEWORK (C) Dextron12, 2025.
// This is an experimental game framework/engine built off of OpenGL.
// Dexium-Framework has been inspired by projects like Hazel(TheCherno Studio), DevDuck's devlogs, ThinMatrix's devlogs
// And most importantly; Joey De Vries amazing book on modern OpenGL LearnOpenGL.com

#ifndef DEXIUM_HPP
#define DEXIUM_HPP

// -- INCLUDES FOR ENTIRE DEXIUM_LIB --
#include <core/WindowContext.hpp>
//#include <GLFW/glfw3.h>
// GLAD is included through WindowContext ( and initialised )
#include <glm/glm.hpp>

#include <memory>
#include <optional>

#include "core/Error.hpp"

#include "core/Layers.hpp"
#include "core/Signal.hpp"

#include "core/Sprite.hpp"
#include "core/AssetManager.hpp"
#include "core/Texture.hpp"
#include "core/SpriteAnimations.hpp"
#include "core/VFS.hpp"


#include "fmt/color.h"


// Selective Dexium API exposure
using Dexium::TraceLog;


// using Sprite = Dexium::Sprite;
using VFS = Dexium::VFS;

using AssetManager = Dexium::AssetManager;
using Sprite = Dexium::Sprite;
using Spritesheet = Dexium::Spritesheet;

using GameLayer = Dexium::AppState;
using AppLayer = Dexium::AppState;
using Overlay = Dexium::Overlay;


// Meyers-Singelton
class EngineState {
public:
    static void init();
    static void shutdown();
    static void run(); // Will run an internal-loop, manage engine state + lifetime and execute currently bound Layer each frame

    static EngineState& get();

    // Provide Direct access for simple vars

    // Getter/Setter funcs for complex vars/critical systems
    static const bool& getAppState(); // Determines if app is running or not. Internally the engine configures this and hence cannot be overwritten by the end-user. use glfwWindowShouldClose() to close your application

    //Window life-time management
    static void attachWindow(const std::string& windowTitle, const int windowWidth, const int windowHeight);
    static void detachWindow(); // Frees the WindowContext bound to the active EngineState
    static Dexium::WindowContext* getWindow();

    // Takes a vec4{} and stores it to be used as the bgColor when EngineState::run() is called
    static void setBgColor(const glm::vec4& color);
    static void setBgColor(const glm::vec3& color);

    // We're already using FMT for CXX20 *print* function, why not make use of its pre-defined colours too!
    // Takes a FMT::Colour and converts it to a normalized vec4{} to be used as the BG col when calling EngineState::run()
    static void setBgColor(const fmt::rgb& color);

    static float getDeltaTime();

    /* Layer management */
    static void addLayer(const std::string& layerID, std::shared_ptr<Dexium::AppState> layer);

    // Allows end-users to directly create a layer from an object/ref
    template<typename T>
    static void addLayer(const std::string& layerID, T* layer) {
        if (!layer) {
            TraceLog(Dexium::LOG_ERROR, "Cannot add null layer '{}'", layerID);
            return;
        }

        // Runtime check to make sure T is derived from AppState
        if constexpr (!std::is_base_of<Dexium::AppState, T>::value) {
            TraceLog(Dexium::LOG_ERROR, "Layer '{}' must derive from AppState", layerID);
            return;
        }

        addLayer(layerID, std::shared_ptr<T>(layer));
    }

    template<typename T>
    static void addLayer(const std::string& layerID, std::shared_ptr<T> layer) {
        static_assert(std::is_base_of<Dexium::AppState, T>::value, "Layer must derive from AppState");
        addLayer(layerID, std::static_pointer_cast<Dexium::AppState>(layer));
    }

    // Activates/deactivates a layer. Param 1 is for new layer, param 2 take optionals econd layer (parse std::nullopt, if you dont whish to pause prev layer), 3rd paam is to provide a custom transition script (executes before transition)
    static void SwapLayer(const std::string& ID, std::optional<std::string> oldLayerID = std::nullopt, std::function<void()> transitionScript = nullptr);

    //User-level Signals: (user can add listener to handle event)
    Signal<int, int> sig_windowResized; // Triggered when window attached to WindowContext resizes
private:
    EngineState();

    ~EngineState();

    // Friend classes. Only engine sub-systems can modify engine state internals, they are a part of the engine
    friend class Dexium::WindowContext;
    //friend class Dexium::AppState; // AppState needs to trust EngineState

    // Prevent copying
    EngineState(const EngineState&) = delete;
    EngineState&operator=(const EngineState&) = delete;

    // Engine-level & controlled vars:
    bool appState;
    std::unique_ptr<Dexium::WindowContext> _windowContext;

    // Engine Level initated flags
    bool GLFW_INIT = false;
    bool GLAD_INIT = false;
    bool VFS_INIT = false;

    bool ENGINE_HEADLESS = false; // only TRUE if the engine is configured to run without a window (in headless mode)(Rendering in headless mode is impossible)

    // deltaTime
    float _deltaTime;
    std::chrono::steady_clock::time_point _lastFrameTime;

    // Window Colour
    glm::vec4 _scrCol = glm::vec4(0.0f);

    // Layer Configurations
    std::unordered_map<std::string, std::shared_ptr<Dexium::AppState>> _layers;




};

#endif

