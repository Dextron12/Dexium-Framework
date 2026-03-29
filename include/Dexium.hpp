
#ifndef DEXIUM_HPP
#define DEXIUM_HPP

// Utils
#include <core/Error.hpp> // Provides TraceLog and Logging features
#include <core/Colour.h> // Provies Colour

#include <core/Layers.hpp>
#include <core/VFS.hpp>

#include <core/Shader.hpp>
#include <core/Mesh.hpp>

#include <core/Camera.hpp>

#include <renderer/viewport.hpp>
#include <renderer/RenderTarget.hpp>



#include <memory>
#include <unordered_map>
#include <vector>

#include "renderer/Renderer.hpp"
#include "core/windowContext.hpp"

using Shader = Dexium::Core::Shader;
using VFS = Dexium::Core::VFS;
using Material = Dexium::Core::Material;
//using Renderer = Dexium::Core::Renderer;

using GameLayer = Dexium::Core::AppState;
using AppLayer = Dexium::Core::AppState;

class EngineState {
public:
    static void init();
    static void run(); // runs the engines internal loop
    static void shutdown();

    static EngineState& get();

    // Window lifetime management
    static void attachWindow(const std::string& windowTitle, int windowWidth, int windowHeight);
    static void detachWindow();
    static Dexium::Core::WindowContext& getWindowContext();

    // Layer management
    static void addLayer(const std::string& layerID, std::unique_ptr<Dexium::Core::AppState> layer);

    Dexium::Renderer::Viewport* getDefaultViewport();

    // Logger Sub-system. Create a new logger with createLogger()
    std::unique_ptr<Dexium::Core::Logger> logger; // If no logger is present in debug build a panic msg will be printed before supression, otherwise in REL builds logs will automatically be suppressed
private:
    EngineState();
    ~EngineState() = default;

    // Prevent copying
    EngineState(const EngineState&) = delete;
    EngineState& operator=(const EngineState&) = delete;

    // Internal loop bool
    bool m_appState = false;

    std::vector<std::string> m_activeLayers;

    // Complex storage types
    std::unordered_map<std::string, std::unique_ptr<Dexium::Core::AppState>> m_layers;

    std::unique_ptr<Dexium::Core::WindowContext> windowContext;

    std::unique_ptr<Dexium::Initializers::glfwInitializer> glfwInit;

};

void clearColour(Colour colour);

// outputStreams dictate where the logger outputs its messages, multiple outputs can be used.
// format dictates the style, features and colour that the logs are output in
// These define the state level operations of the Logger, but can also be indvidually overriden per log
void createLogger(Dexium::Utils::LoggerOutput outputStreams, Dexium::Utils::LoggerFormat format);

#endif