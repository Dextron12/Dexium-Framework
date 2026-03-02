//
// Created by ethan on 23/2/26.
//

#ifndef DEXIUM_RENDERER_HPP
#define DEXIUM_RENDERER_HPP

#include <core/Colour.h>
#include <core/BitwiseFlag.hpp>

#include <renderer/RenderTarget.hpp>

#include "glad/gl.h"

#include <array>

// Forward declares for ptr declaration in RenderCommand
namespace Dexium::Core {
    class Mesh;
    class Material;
    class Transform;

    class Texture;

    class windowContext;


    // Bitwise flag enum for the different buffers
    enum class bufferTargets {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2
    };

    //Enable BitwiseFlag operations for bufferTargets
    template <>
    struct EnableBitmaskOperators<bufferTargets> {
        static constexpr bool value = true;
    };

    //Bitwise flag enum for which pass to do
    enum class RenderPass {
        None = 0,
        Opaque = 1 << 0,
        Transparent = 1 << 1
    };
    template <>
    struct EnableBitmaskOperators<RenderPass> {
        static constexpr bool value = true;
    };




    struct RenderCommand {
        RenderState::RenderTarget* target;
        RenderPass pass;

        Mesh* mesh;
        Material* material;

        Transform* transform;

        RenderCommand(RenderState::RenderTarget* target, Mesh* mesh, Material* material, Transform* transform, RenderPass pass = RenderPass::Opaque);
    };

    class Renderer {
    public:
        Renderer(windowContext* windowCtx);

        // Make Renderer non-copyable BUT movable.
        // Movable renderer is needed becasue AppLayer ustes std::move, meaning all variables scoped within it also msut be movable
        Renderer(Renderer&& other) noexcept
        : m_winCtx(other.m_winCtx)
        , m_OpaqueComms(other.m_OpaqueComms)
        , m_TransparentComms(other.m_TransparentComms)
        , m_bufferTargets(other.m_bufferTargets)
        , m_maxTextureSlots(other.m_maxTextureSlots) {
            // ensure that moved-from Renderer does not implicitly call the winCtx dctor() on the winCtx
            other.m_winCtx = nullptr;

            //Clear other.commands
            other.m_OpaqueComms.clear();
            other.m_TransparentComms.clear();
        }

        // Move assignment
        Renderer& operator=(Renderer&& other) noexcept {
            if (this != &other) {
                m_winCtx = other.m_winCtx;
                m_OpaqueComms = other.m_OpaqueComms;
                m_TransparentComms = other.m_TransparentComms;
                m_bufferTargets = other.m_bufferTargets;

                other.m_winCtx = nullptr;
                other.m_OpaqueComms.clear();
                other.m_TransparentComms.clear();
            }
            return *this;
        }

        // Prevent copies
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        /* Render Order
         * - Opaque Pass
         * RenderTarget
         * Shader
         * Pipeline state
         * Material
         * - Transparent pass
         * RenderTarget
         * Z (descending)
         */

        int pollHW_MaxTexSlots() const;

        // Prepares a command(Model) for rendering
        void submit(const RenderCommand& cmd);

        // Updates the renderer (Call this ONCE at the end of each render frame)
        void flush();

        // Quality of Life fns
        void setClearColor(Color color, bufferTargets buffs = bufferTargets::Color);

        // generally this fn shouldn't be called, unless you need to explcitly clear a buffer. But youc an pass multple buffers toc lear when you set the renderer colour
        void clearBuffers(bufferTargets targets);

    private:
        windowContext* m_winCtx; // Non-owning ptr, very unlikely to be derefrenced before the renderer

        //Stores the render comm submissions (Cleared after each frame)
        std::vector<RenderCommand> m_OpaqueComms;
        std::vector<RenderCommand> m_TransparentComms;

        // Store the state buffers
        bufferTargets m_bufferTargets;

        //Store the MAX supported texture units (Polled at Renderer ctor)
        int m_maxTextureSlots;
        int m_nextTextureSlot = 1; // Leave TEXTURE0 for fallack/default texture
        std::vector<Texture*> m_boundTextures; // Determiens What is CURRENTLY bound to slot N
        std::unordered_map<Texture*, int> m_batchLookup; // Storres the lookups of textures (Stored per batch/drawCall)

        //Store active viewport
        RenderState::Viewport m_activeViewport = {};
        //Store active shader
        int m_activeShader = 0;
        //Store next texture slot
        GLenum m_activeTexture = 1; // 0 reserved for fallback texture


        // Colour and stateBuffers
        Color stateCol = {0,0,0,0};
        GLbitfield bufferMasks;

        // Remove default construciton of Renderer
        Renderer() = delete;
    };


}

#endif //DEXIUM_RENDERER_HPP