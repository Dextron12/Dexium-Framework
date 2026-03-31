//
// Created by ethan on 29/3/26.
//

#ifndef DEXIUM_RENDERPASS_HPP
#define DEXIUM_RENDERPASS_HPP

#include <utils/BitwiseFlag.hpp>

#include <core/Colour.h>

#include <glad/gl.h>

#include <renderer/Command.hpp>

// forward declares
namespace Dexium::Core {
    class baseCamera;
    class Mesh;
    class Material;
    class Transform;

}
namespace Dexium::Renderer{
    class RenderTarget;
    class PipelineState;
};


namespace Dexium::Utils {

    enum class BufferTarget
    {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2,
    };

    // Enable bitwise operations
    template <>
    struct EnableBitmaskOperators<BufferTarget> {
        static constexpr bool value = true;
    };

}

namespace Dexium::Renderer {
    //Forward-declare for RenderCommand (Renderer::Command)
    //struct Command;
    //Forward-declare for Renderer (Renderer::Renderer)
    class Renderer;

    // The state-level data that configures GL state per render pass
    class PipelineState {
    public:
        bool DepthTest = true; // Essentially about visability of pixels. Think of it as the culling space
        bool DepthWrite = true; // Decides if its depth result effects future fragments
        bool blending = false;

        std::string Projection_uName; // The uniform name for the Projection(mat4) in the shader program
        std::string View_uName; // The uniform name for the View(mat4) in the shader program
        std::string Model_uName; // The uniform name for Model(mat4) in the shader program

        GLenum depthFunc = GL_LESS;

        struct BlendMode {
            GLenum src;
            GLenum dst;
        } blendMode = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};

        Utils::BufferTarget buffers = Utils::BufferTarget::None;
        Colour passColor = {0,0,0,0};
    };



    class RenderPass {
    public:
        RenderPass(RenderTarget* renderTarget, Core::baseCamera* camera);
        RenderPass() = default; // SHOULD ONLY BE USED FOR TYPE INITIALIZATION!!

        void storeCommand(Core::Mesh* mesh, Core::Material* material, Core::Transform* transform);
        void storeCommand(const Command& cmd);

        void clearCommands();

        void setClearColor(const Colour color);

        std::string passName;

        PipelineState plpState;

    protected:
        std::vector<Command> m_commands;

        RenderTarget* renderTarget = nullptr; // Accessor to VP data and future FBO target
        Core::baseCamera* camera = nullptr; // Accessor to camera View and Proj matrices


        friend Renderer; // Allow Renderer to access protected vars
        // RenderPass ultimately stores such data as protected to prevent end-user modifying state


    };


}




#endif //DEXIUM_RENDERPASS_HPP