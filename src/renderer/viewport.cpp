//
// Created by Dextron12 on 01/01/26
//

#include <renderer/viewport.hpp>

namespace Dexium::RenderState {

    Viewport::Viewport() : x(0), y(0), w(0), h(0) {
    }

    Viewport::Viewport(const int& x, const int& y, const int& w, const int& h)
        : x(x), y(y), w(w), h(h) {
    }
}