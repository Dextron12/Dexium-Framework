//
// Created by Dextron12 on 1/12/25.
//

#include <core/Layers.hpp>

namespace Dexium::Core {

    void AppState::run() {
        if (!isInited) {
            onInit();
            isInited = true;
            isRunning = true;
        }
        if (!isRunning) {
            return; // Layer is paused/inactive
        }

        onUpdate();
        onRender();
    }

    void AppState::RequestPause() {
        if (!_isShutdown) {
            isRunning = !isRunning;
        }
    }

    void AppState::RequestShutdown() {
        _isShutdown = true; // If engine detects this, it will safely remove it from the stack
    }




}