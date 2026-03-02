//
// Created by Dextron12 on 3/3/26.
//

#ifndef DEXIUM_RENDERTARGET_HPP
#define DEXIUM_RENDERTARGET_HPP

#include <renderer/viewport.hpp>

namespace Dexium::RenderState {

    // Eventually plan to add FBOs
    class RenderTarget {
    public:
        RenderTarget() = delete;
        RenderTarget(const RenderTarget&) = delete;

        //Takes ownership of the viewport, suggest you create the viewport within the param args
        explicit RenderTarget(const Viewport&& vp)
            : m_viewport(std::move(vp)) {}

        Viewport m_viewport;
    };
}

#endif //DEXIUM_RENDERSTATE_HPP