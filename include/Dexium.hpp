
#ifndef DEXIUM_HPP
#define DEXIUM_HPP

#include <core/windowContext.hpp>

class EngineState {
public:
    static void init();
    static void run(); // runs the engines internal loop
    static void shutdown();

    static EngineState& get();

    // Window lifetime management
    static void attachWindow(const std::string& windowTitle, int windowHeight, int windowWidth);
    static void detachWindow();

private:
    EngineState();
    ~EngineState();

    // Friend WindowContext (Engine Internal WIndow Manager)
    friend class Dexium::Core::windowContext;

    // Prvent copying
    EngineState(const EngineState&) = delete;
    EngineState& operator=(const EngineState&) = delete;

    // Internal loop bool
    bool m_appState = false;
};


#endif