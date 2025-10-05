// Dexium-Framework. (C) 2025. Dextron12



#include <core/Layers.hpp>

#include "Dexium.hpp"
#include "core/Error.hpp"

Dexium::Layer::Layer(std::string LayerID) {
    auto& ctx = EngineState::get();

    auto& layerRef = ctx._layers;
    if (layerRef.find(LayerID) != layerRef.end()) {
        // An existing layer fo the same ID was found. Reject new one
        TraceLog(LOG_WARNING, "{} is already an existing layer. Cannot overwrite layer. Call shutdown() on {} first", LayerID);
        return;
    }

    ID = LayerID;


}



void Dexium::Layer::run() {
    if (!init) {
        if (!onInit) {
            TraceLog(LOG_WARNING, "No init function is bound for Layer: {}", ID);
        } else {
            // Call the init function
            onInit();
        }
    }

    if (!onUpdate) {
        TraceLog(LOG_WARNING, "No OnUpdate bound to Layer: {}", ID);
    } else {
        onUpdate();
    }

    if (!onRender) {
        TraceLog(LOG_WARNING, "No OnRender() bound to Layer: {}", ID);
    } else {
        onRender();
    }
}
