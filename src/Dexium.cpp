//
// Created by Dextron12 on 1/12/25.
//

#include "Dexium.hpp"

EngineState::EngineState() {
    //Init GLFW
    glfwInit = std::make_unique<Dexium::Core::glfwInitializer>();
    glfwInit->init();

    m_appState = true; // Checks after this ln will determine if app runs

    // Check init state and confure runtime
    if (!glfwInit->isValid()) m_appState = false;

    // Init VFS
    Dexium::Core::VFS::init();
}

EngineState &EngineState::get() {
    static EngineState instance;
    return instance;
}

void EngineState::init() {
     get(); // Forces init
}

void EngineState::shutdown() {
    get().glfwInit = nullptr;
}

void EngineState::attachWindow(const std::string &windowTitle, int windowWidth, int windowHeight) {
    get().windowContext = std::make_unique<Dexium::Core::windowContext>(windowTitle, windowWidth, windowHeight, get().glfwInit.get(), nullptr);
}

void EngineState::detachWindow() {
    get().windowContext = nullptr;
}

Dexium::Core::windowContext& EngineState::getWindowContext() {
    return *get().windowContext;
}

void EngineState::addLayer(const std::string &layerID, std::unique_ptr<Dexium::Core::AppState> layer) {
    //Check if a layer of same ID exists
    auto& ctx = get();
    if (ctx.m_layers.find(layerID) == ctx.m_layers.end()) {
        ctx.m_layers[layerID] = std::move(layer);
        // If no active layers, automatically move this to active
        if (ctx.m_activeLayers.size() == 0) {
            ctx.m_activeLayers.emplace_back(layerID);
            // And then enable it
            ctx.m_layers[layerID]->RequestPause(); // Counter-intuitavie, but intrnally activates/pauses the state
        }
    } else {
        std::cout << "Layer already exists" << std::endl;
    }
}


void EngineState::run() {
    // Check state
    auto& ctx = get();

    if (!ctx.glfwInit->isValid()) {
        std::cout << "NO GLFW context initalised" << std::endl;
        ctx.m_appState = false;
    }

    if (!ctx.windowContext) {
        std::cout << "NO Window context initalised" << std::endl;
        ctx.m_appState = false;
    }

    while (ctx.m_appState) {


        ctx.windowContext->startFrame();

        // Execute layers
        for (auto it = ctx.m_activeLayers.begin(); it != ctx.m_activeLayers.end(); ) {
            auto layer = ctx.m_layers.find(*it);
            if (layer == ctx.m_layers.end()) {
                // The stored ID has been removed elsewhere! WARN: and remove ID
                std::cout << "Layer not found" << std::endl;
                ctx.m_activeLayers.erase(it);
            }

            //layer->first = layerID (currently not needed)

            // Layer found, execute it
            auto& ptr = layer->second;

            if (ptr->isActive() && !ptr->isOverlay()) {
                //Execute regular layer
                ptr->run();
            }

            // Check for shutdown request
            if (ptr->isShuttingDown()) {
                //Stop running/Pause it -> sets the internal isRunning to false
                // If isShutdown = true && isRunning == false, engine treats it as tiome to remove layer from stack
                ptr->RequestPause();
            }
            if (ptr->isShutdown()) {
                ptr->onShutdown();
                // Layer is ready for removal
                it = ctx.m_activeLayers.erase(it);
                // Remove the actual layer
                ctx.m_layers.erase(ctx.m_layers.find(*it));
                continue;
            }

            // Advances to next layer
            ++it;

            // Need to EVENTUALLY add the Overlay layer executer here
            // They should be done differently to ensure Overlay always renders ontop, regardless of hwo many consecutive layers there are

        }

        ctx.windowContext->endFrame();
    }
}



void createLogger(Dexium::Core::LoggerOutput outStreams, Dexium::Core::LoggerFormat format) {
    //Fethc engine context
    auto& logSvs = Dexium::Core::LogService::use();

    if (!logSvs) {
        // NO logger currently exists
        logSvs = std::make_unique<Dexium::Core::Logger>();
        // Logger class has no explicit ctr that takes the required inputs, manually assign instead
        logSvs->outputs = outStreams;
        logSvs->format = format;

#ifdef DEBUG
        TraceLog(LogLevel::DEBUG, "[CreateLogger]: Logger sub-system successfully initialised!");
        #endif
    } else {
        // Logger already exists, output warning of multiple calls 9If in debug)
#ifdef DEBUG
        TraceLog(LogLevel::WARNING, "[CreateLogger]: Sub-system already initialised, are you calling this function multiple times?\nSet ctx.logger = nullptr if you want to delete");
#endif
    }
}


Dexium::RenderState::RenderTarget* createDefaultRenderTarget() {
    //Grab engine context
    auto& ctx = EngineState::get();

    //WindowCtx created a default viewport on creation,a ccess and return it
    return ctx.getWindowContext().getRenderTarget();
}



