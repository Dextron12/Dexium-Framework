// DEXIUM-FRAMEWORK (C) Dextron12, 2025.
// This is an experimental game framework/engine built off of OpenGL.
// Dexium-Framework has been inspired by projects like Hazel(TheCherno Studio), DevDuck's devlogs, ThinMatrix's devlogs
// And most importantly; Joey De Vries amazing book on modern OpenGL LearnOpenGL.com


#include <Dexium.hpp>
#include <optional>
#include <nlohmann/detail/iterators/iteration_proxy.hpp>

#include "core/Error.hpp"
#include "core/helpers.hpp"
#include "core/WindowContext.hpp"
#include "core/versionControl.hpp"

#ifdef DEXIUM_USING_ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

void EngineState::init() {
    get(); // Forces init
}

void EngineState::shutdown() {
    // Shutdown code here
    auto& engine = get();

    engine.appState = false;

    // Optional Package: dear ImGui (Shutdown ImGui)
#ifdef DEXIUM_USING_ImGui
    if (engine.ImGui_Inited) {
        //ImGui_ImplOpenGL3_Shutdown();
        //ImGui_ImplGlfw_Shutdown();
        //ImGui::DestroyContext();

        // Something else is destroying the ImGui context before this code is reached? Above code results in Segfault.
    }
#endif
}

EngineState &EngineState::get() {
    static EngineState instance;
    return instance;
}

EngineState::EngineState() {
    // Eninge startup/initlisation here...

    //Define engine-level signals here
    auto& sm = Dexium::SignalManager::get();
    sm.createSignal<bool>("GLFW_Init");
    sm.createSignal<bool>("GLAD_Init");
    sm.createSignal<bool>("VFS_Init");

    //Listeners
    sm.connect<bool>("GLFW_Init", [this](bool state) {
        if (state) {
            this->GLFW_INIT = true;
            TraceLog(Dexium::LOG_INFO, "[Engine Startup]: GLFW successfully initialized");
        } else {
            this->GLFW_INIT = false;
            const char* err; glfwGetError(&err);
            if (err) {
                TraceLog(Dexium::LOG_FATAL, "[Engine Startup]: GLFW failed to initialise. Reason: \n{}", err);
            } else {
                TraceLog(Dexium::LOG_FATAL, "[Engine Startup]: GLFW failed to initialize. Reason unknown");
            }
        }
    });

    sm.connect<bool>("GLAD_Init", [this](bool state) {
        if (state) {
            TraceLog(Dexium::LOG_INFO, "[Engine Startup]: GLAD successfully initialized");
        } else {
            TraceLog(Dexium::LOG_FATAL, "[Engine Startup]: GLAD failed to initialize");
        }
        this->GLAD_INIT = state;
    });

    sm.connect<bool>("VFS_Init", [this](bool state) {
        if (state) {
            TraceLog(Dexium::LOG_INFO, "[Engine Startup]: Application running from directory: {}", VFS::getExecutablePath().string());
        } else {
            TraceLog(Dexium::LOG_WARNING, "[Engine Startup]: Failed to establish working directory or init filesystem. relative paths may break");
        }
        this->VFS_INIT = state;
    });

    //Init GLFW
    if (glfwInit() == GLFW_FALSE) {
        sm.emit("GLFW_Init", false);
    } else {
        sm.emit("GLFW_Init", true);
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // DO NOT CALL ANY glad FUNCTIONS HERE, it is initalised after a windowContext is created

    // Set GLFW ErrorCallback
    glfwSetErrorCallback([](int error, const char* description) {
        TraceLog(Dexium::LOG_TRACE, "GLFW Error[Code: {}]: {}", error, description);
    });

    // Init VFS (Virtual File System). Will fetch the execPath
    VFS::init();

    // Optional Package: ImGui
#ifdef DEXIUM_USING_ImGui
    ImGui_Inited = false;
#endif

    // Register AssetManager Default Loaders
    AssetManager::getInstance().registerDefaultLoaders();
    TraceLog(Dexium::LOG_INFO, "[Engine Startup]: Default Loaders for AssetManager have been registered");


    TraceLog(Dexium::LOG_INFO, "[Dexium-Core]: Initialized. Using Dexium: {}.{}.{}", Dexium::VERSION::major, Dexium::VERSION::minor, Dexium::VERSION::patch);

    appState = false; // remains false, until a WindowContext is created and succeeds
}

EngineState::~EngineState() {

}

void EngineState::InitImGui() {
    // OPTIONAL PACKAGE: Dear ImGui
#ifdef DEXIUM_USING_ImGui
    // Get EngineState context
    auto& ctx = EngineState::get();
    //Check if already initalised
    if (ctx.ImGui_Inited) return; // Early exit, if already inialised
    //Check for a valid WindowContext
    if (ctx._windowContext->window == nullptr) {
        TraceLog(Dexium::LOG_ERROR, "[ImGui Initt]: Cannot init ImGui package without a valid WindowContext");
        return;
    }
    // Create ImGui Context
    ImGui::CreateContext();
    ctx.io = &ImGui::GetIO();
    ctx.io->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable; // Enable ImGui docking and viewports

    // Init ImGui for platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(ctx._windowContext->window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // Initialise ImGui to use OpenGL 3.3 core!!

    // Configure main viewport 9Due to the nature of this viewport, docking external windows onto it isn't available. You'd need to...
    // create your own viewport/dockspace to dock a window onto(and provide docking options)
    auto& viewport = ctx.viewport;
    viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Configure the viewport flags so that the engine rendeer is visible and input is passed through
    ctx.window_flags |= ImGuiWindowFlags_NoTitleBar
    | ImGuiWindowFlags_NoCollapse
    | ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoMove
    | ImGuiWindowFlags_NoBringToFrontOnFocus
    | ImGuiWindowFlags_NoNavFocus
    | ImGuiWindowFlags_NoBackground;

    TraceLog(Dexium::LOG_INFO, "[Engine Startup]: Dear ImGui initialized. Version: {}", ImGui::GetVersion());
    ctx.ImGui_Inited = true;
#endif
}



void EngineState::run() {
    // Check state configurations
    auto& ctx = get();

    if (!ctx.GLFW_INIT || !ctx.GLAD_INIT) {
        if (!ctx.GLAD_INIT && ctx.ENGINE_HEADLESS) return; // Allows check to be nullified if running in headless mode
        TraceLog(Dexium::LOG_FATAL, "One or more core libraries are not initialised\nDid you forget to call Dexium::init()?");
    }

    if (!ctx.VFS_INIT) {
        TraceLog(Dexium::LOG_WARNING, "The Virtual FileSystem is not initialized. Many sub-systems depend on the executabe path, without it your application may crash");
    }

    if (!ctx._windowContext && !ctx.ENGINE_HEADLESS) {
        TraceLog(Dexium::LOG_ERROR, "No WindowContext is bound to the engine\nApplication cannot run without a windowContext");
    }

    // Optional Package: Dear ImGui (Lazy init ImGui)
    // Again, this is done because a valid WindowCOntext isnt available during EngineState init
#ifdef DEXIUM_USING_ImGui
    InitImGui();
#endif

    while (ctx.appState) {
        if (!ctx.ENGINE_HEADLESS && ctx._windowContext) {
            // Running with a valid WindwoContext. Update frame
            ctx._windowContext->startFrame();
        }

        // update deltaTime
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - ctx._lastFrameTime;
        ctx._deltaTime = elapsed.count(); // convert to float seconds
        ctx._lastFrameTime = now;

        // Optional Package: Dear ImGui (Rendering)
        // Start a ImGui NewFrame here and end + render after layers have been drawn
#ifdef DEXIUM_USING_ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Force dockspace (This in part allows floating/OS level windows)
        ImGui::Begin("MainDockspace", nullptr, ctx.window_flags);
        ImGui::DockSpace(ImGui::GetID("MainDockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();


#endif


        // Clear screen from prev frame
        glClearColor(ctx._scrCol.x, ctx._scrCol.y, ctx._scrCol.z, ctx._scrCol.w);
        glClear(GL_COLOR_BUFFER_BIT); // Other buffers need to also be managed here

        // execute layers
        for (auto it = ctx._layers.begin(); it != ctx._layers.end(); ) {
            auto& ptr = it->second;
            //auto& name = it->first; // Not currently needed!

            if (ptr->isActive() && !ptr->isOverlay()) {
                // Execute regular layer
                ptr->run();
            }

            // Check for shutdown requests
            if (ptr->isShuttingDown()) {
                // Stop running (pause it -> sets internal isRunning to false
                // If isShutdown = true && isrunning == false, engine treats this as time to remove it from stack
                ptr->RequestPause();
            }
            if (ptr->isShutdown()) {
                ptr->onShutdown();
                // Layer is ready for removal
                it = ctx._layers.erase(it);
                continue;
            }

            ++it; // Advance to next layer
        }


        // Now render overlays ontop of current layers
        for (auto it = ctx._layers.begin(); it != ctx._layers.end(); ) {
            auto& ptr = it->second;
            //auto& name = it->first; // Again, not currently needed

            if (ptr->isActive() && ptr->isOverlay()) {
                ptr->run();
            }

            // Check for shutdown signal
            if (ptr->isShuttingDown()) {
                ptr->RequestPause();
            }
            if (ptr->isShutdown()) {
                it = ctx._layers.erase(it);
                continue;
            }

            ++it; // advance to next layer
        }


        //Optional Package: Dear ImGui (Render state)
#ifdef DEXIUM_USING_ImGui
        // Render ImGuia fter everything else
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // update and render viewports(Allows OS level windows, especially on Windows platform)
        if (ctx.io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_context);
        }

#endif

        // end frame
        if (!ctx.ENGINE_HEADLESS && ctx._windowContext) {
            ctx._windowContext->endFrame();
        }
    }
}

void EngineState::attachWindow(const std::string &windowTitle, const int windowWidth, const int windowHeight) {
    get()._windowContext = std::make_unique<Dexium::WindowContext>(windowTitle, windowWidth, windowHeight);
}

void EngineState::detachWindow() {
    if (get()._windowContext) {
        // reset ptr
        get()._windowContext.reset();
        TraceLog(Dexium::LOG_WARNING, "Active WindowContext has been detached");
    } else {
        TraceLog(Dexium::LOG_WARNING, "No WindowContext is attached. Cannot detach one");
    }
}

Dexium::WindowContext* EngineState::getWindow() {
    auto& ctx = get();
    if (!ctx._windowContext) {
        TraceLog(Dexium::LOG_WARNING, "Cannot access a windowContext, none is bound to the engine!");
    }
    return ctx._windowContext ? ctx._windowContext.get() : nullptr;
}

glm::vec2 EngineState::getWindowSize() {
    auto& ctx = get();
    if (!ctx._windowContext) {
        TraceLog(Dexium::LOG_WARNING, "Cannot access a windowContext, none is bound to the engine!");
    }
    return glm::vec2(ctx._windowContext->width, ctx._windowContext->height);
}


void EngineState::setBgColor(const glm::vec4 &color) {
    //convert FMT colour to vec colour
    get()._scrCol = glm::vec4(
        color.x / 255.0f,
        color.y / 255.0f,
        color.z / 255.0f,
        color.w / 255.0f);
}

void EngineState::setBgColor(const glm::vec3 &color) {
    get()._scrCol = glm::vec4(
        color.x / 255.0f,
        color.y / 255.0f,
        color.z / 255.0f,
        1.0f);
}

void EngineState::setBgColor(const fmt::rgb& color) {
    get()._scrCol = glm::vec4(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        255.0f);
}


float EngineState::getDeltaTime() {
    return get()._deltaTime;
}

void EngineState::addLayer(const std::string& layerID, std::shared_ptr<Dexium::AppState> layer) {
    //check if layerID already exists:
    const auto& layers = get()._layers;
    if (layers.find(layerID) == layers.end()) {
        // No match, create new layer
        get()._layers.emplace(layerID, layer);
    } else {
        TraceLog(Dexium::LOG_WARNING, "Cannot cerate new layer: {}, it already exists!", layerID);
    }

    //Check if ONLy one layer(if so, make it the active layer)(So end-user doesn';t have to manually activate it!)
    if (get()._layers.size() == 1) {
        //One layer exists (the newly created layer)
        SwapLayer(layerID);
    }
}

void EngineState::SwapLayer(const std::string &layerID, std::optional<std::string> oldLayerID, std::function<void()> transitionScript) {
    // First check if layer exists
    const auto& layers = get()._layers;

    if (layers.find(layerID) == layers.end()) {
        TraceLog(Dexium::LOG_WARNING, "Cannot transition to layer {} as it does not exist within the engine", layerID);
        return;
    }

    // Layer exists, run script if provided
    if (transitionScript != nullptr) {
        // execute script
        transitionScript();
    }

    if (oldLayerID.has_value()) {
        auto extLayer = layers.find(oldLayerID.value());
        if (extLayer != layers.end()) {
            // Old layer exists, pause it.
            extLayer->second->RequestPause();
        }
    }

    // Now change to new layer
   layers.find(layerID)->second->RequestPause(); // I know this sounds counter-intuitive -> to 'pause' when we are starting new layer. But requestPause() internally activates/pauses layer dependant on layer state
}



