//
// Created by ethan on 1/4/26.
//

#ifndef DEXIUM_PRESETS_HPP
#define DEXIUM_PRESETS_HPP

// Defines presets used to define a RenderPass and its configurations and other Renderbale states

#include <renderer/RenderPass.hpp>

namespace Dexium::Renderer::Presets {

    // Generates an Opaque RenderPass instance to insert commands into.
    RenderPass OpaquePass(RenderTarget* target, Core::baseCamera* camera, std::string_view passName = {});

    // Generates a transparent RenderPass instance to insert commands into.
    RenderPass TransparentPass(RenderTarget* target, Core::baseCamera* camera, std::string_view passName = {});
}

#endif //DEXIUM_PRESETS_HPP