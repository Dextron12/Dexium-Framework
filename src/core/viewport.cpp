//
// Created by Dextron12 on 01/01/26
//

#include <core/viewport.hpp>

namespace Dexium::Core {

    Viewport::Viewport() : x(0), y(0), w(0), h(0) {
    }

    Viewport::Viewport(const int& x, const int& y, const int& w, const int& h)
        : x(x), y(y), w(w), h(h) {
    }
}

/*
namespace Dexium::Core {

    Viewport::Viewport(const windowContext& windowCtx, int xOffset, int yOffset, int viewportWidth, int viewportHeight, std::optional<std::string> viewportLabel) {
        x = xOffset; y = yOffset;
        width = viewportWidth; height = viewportHeight;

        designWidth = viewportWidth; designHeight = viewportHeight;
        windowBaseWidth = windowCtx.baseWidth; windowBaseHeight = windowCtx.baseHeight;

        // Store viewport label if provided
        if (viewportLabel.has_value()) {
            ID.setLabel(viewportLabel.value());
        }

        // COnnect resize func to global resize signal
        // This listener is destroyed in the viewport destructor
        //Dexium::Globals::Signals::sig_onWindowResize
        entt::sink{Dexium::Globals::Signals::sig_onWindowResize}.connect<&Viewport::resizeViewport>(*this);
    }

    Viewport::~Viewport() {
        //Safely disocnnect the associate dlistener from the global signal
        //To discconect: Need to re-create the sink, then disconnect from it
        entt::sink{Dexium::Globals::Signals::sig_onWindowResize}.disconnect<&Viewport::resizeViewport>(*this);
    }

    void Viewport::changeOffsets(int xOffset, int yOffset) {
        if (!visible) return;

        x = xOffset; y = yOffset;

        //Mark dirty, for the renderer to clean
        dirty = true;
    }

    void Viewport::resizeViewport(int viewportWidth, int viewportHeight) {
        if (!visible) return;

        // Scale proportionally based on design size vs window size
        float scaleX = static_cast<float>(viewportWidth) / windowBaseWidth;
        float scaleY = static_cast<float>(viewportHeight) / windowBaseHeight;

        width = static_cast<int>(designWidth * scaleX);
        height = static_cast<int>(designHeight * scaleY);

        //Mark dirty, renderer will check and clean
        dirty = true;
    }



}

*/