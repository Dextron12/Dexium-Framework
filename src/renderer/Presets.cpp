//
// Created by ethan on 1/4/26.
//


#include <renderer/Presets.hpp>

#include <core/Camera.hpp>

#include <string> // Used for std::to_string

namespace Dexium::Renderer::Presets {
    RenderPass OpaquePass(RenderTarget* target, Core::baseCamera* camera, std::string_view passName) {
        // A bit of a hack (single-threaded), consider using std::atomic<int> if thread-safety is a concern
        static int opqCount = 0;

        RenderPass pass(target, camera);

        // Generate a default passName or use provided:
        pass.passName = (passName.empty()) ? "OpaquePass_" + std::to_string(opqCount++) : passName.data(); // SHould eventually use a ref that we resolve (with the manager) here so lower level systems can continue to sue raw ptr's (but they are garanteed to exist that way)

        // Set defaults for MVP uniforms (u(uniform)_name) is the default name schema
        pass.plpState.Projection_uName = "u_Projection";
        pass.plpState.View_uName = "u_View";
        pass.plpState.Model_uName = "u_Model";

        // Set clearColor to a dark grey
        pass.setClearColor({Colour::fromBytes(47, 47, 47, 255)});

        // enable colour buffer & depth buffers
        pass.plpState.buffers |= Utils::BufferTarget::Color | Utils::BufferTarget::Depth;

        return pass;
    }

    RenderPass TransparentPass(RenderTarget* target, Core::baseCamera* camera, std::string_view passName) {
        static int transCount = 0; // Again, consider using atomic_)var if thread-safety is a concern!!

        RenderPass pass(target, camera); // SHould eventually use a ref that we resolve (with the manager) here so lower level systems can continue to sue raw ptr's (but they are garanteed to exist that way)

        // Generate a default passName or use provided:
        pass.passName = (passName.empty()) ? "TransparentPass_" + std::to_string(transCount++) : passName.data();

        pass.plpState.Projection_uName = "u_Projection";
        pass.plpState.View_uName = "u_View";
        pass.plpState.Model_uName = "u_Model";


        // enable blending
        pass.plpState.blending = true;

        // Set clearColor to a dark grey
        pass.setClearColor({Colour::fromBytes(47, 47, 47, 255)});

        // End-user/program should update the blendMode if they aren't happy with the built-in default

        return pass;

    }
}

