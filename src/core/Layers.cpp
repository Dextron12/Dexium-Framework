// Dexium-Framework. (C) 2025. Dextron12



#include <core/Layers.hpp>



void Dexium::AppState::run() {
    if (!isInited) {
        onInit();
        isInited = true;
        isRunning = true;
    }
    if (!isRunning) {
        return; // layer is paused or inactive
    }

    onUpdate();
    onRender();


}


void Dexium::AppState::RequestPause() {
    if (!_isShutdown) {
        isRunning = !isRunning;
    }
}

void Dexium::AppState::RequestShutdown() {
    _isShutdown = true; // If engine detects this, it will *safely* remove the layer from the stack
}