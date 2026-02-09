//
// Created by Dextron12 on 31/12/25.
//

#include <utils/ID.hpp>

#include <core/windowContext.hpp>

#ifndef DEXIUM_VIEWPORT_HPP
#define DEXIUM_VIEWPORT_HPP

namespace Dexium::Core {
    class Renderer;
}

namespace Dexium {

    namespace Core {


        class Viewport {
        public:
            int x, y, width, height;
            bool visible; // Determines if the viewport is active or not.
            Dexium::Utils::IDHandle ID;


            // Should eventuallys tore a handle to a FBO or lsit of FBOs as render targets

            // X/Y offsets are applied from top-left of screen (0,0) -> (screenWidth, screenHeight)
            // viewportWidth & height set the size of the scalable viewport if less than the screen size
            Viewport(const windowContext& windowCtx, int xOffset, int yOffset, int viewportWidth, int viewportHeight, std::optional<std::string> viewportLabel = std::nullopt);
            Viewport()
                : x(0), y(0), width(0), height(0), visible(false) {}
            // Safely disconnects the internal listener from the global sig_onWindowResize
            ~Viewport();

            void changeOffsets(int xOffset, int yOffset);

            bool isDirty() const {return dirty; }
            void clearDirty() {dirty = false; }

            // Engine will internally call this func on a window resize
            // A call to this func invokes glViewport()
            void resizeViewport(int viewportWidth, int viewportHeight);

        protected:
            //Friend the renderer
            friend Dexium::Core::Renderer;
            // A flag to help determine fi the viewport needs to be resized
            // The renderer will check this flag and act on it
            bool dirty = false;

            int designWidth, designHeight; // The designed(inital) size of the viewport
            int windowBaseWidth, windowBaseHeight;
        };


    }

}

#endif //DEXIUM_VIEWPORT_HPP