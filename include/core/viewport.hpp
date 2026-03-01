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


namespace Dexium::Core {

    class Viewport {
    public:
        int x; int y; int w; int h;

        Viewport();
        Viewport(const int& x_, const int& y_, const int& w_, const int& h_);

        bool operator==(const Viewport& other) const {
            return x == other.x &&
                y == other.y &&
                w == other.w &&
                h == other.h;
        }

        bool operator!=(const Viewport& other) const {
            return !(*this == other);
        }
    };
}


#endif //DEXIUM_VIEWPORT_HPP