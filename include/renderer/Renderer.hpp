//
// Created by ethan on 23/2/26.
//

#ifndef DEXIUM_RENDERER_HPP
#define DEXIUM_RENDERER_HPP

#include <core/Colour.h>
#include <utils/BitwiseFlag.hpp>

#include <renderer/RenderTarget.hpp>

#include "glad/gl.h"

#include <array>

// The general design of the renderer:
/*
 * Renderer = Execution Engine
 * RenderPass = configuration
 * RenderCommand = pure data of what shjould be rendered.
 * Its important that RenderPass & RenderCommand do not overlap in any whay
 */

// Forward declares for ptr declaration in RenderCommand
namespace Dexium::Core {
    class Mesh;
    class Material;
    class Transform;

    class Texture;
    class baseCamera;

    class WindowContext;
}

namespace Dexium::Renderer {
    //Forward-declare of Renderer::RenderPass
    class RenderPass;

    class Renderer {
    public:
        Renderer();

        // Make Renderer non-copyable BUT movable.
        // Movable renderer is needed becasue AppLayer ustes std::move, meaning all variables scoped within it also msut be movable
        Renderer(Renderer&& other) noexcept {
        }

        // Move assignment
        Renderer& operator=(Renderer&& other) noexcept {
            if (this != &other) {
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

        // Prepares a batched pass of commands for rendering (A set of batched models in their configured GL state)
        void submit(RenderPass* pass);

        // Updates the renderer (Call this ONCE at the end of each render frame)
        void flush();

    private:

        //Store the MAX supported texture units (Polled at Renderer ctor)
        int m_maxTextureSlots;
        int m_nextTextureSlot = 1; // Leave TEXTURE0 for fallack/default texture
        std::vector<Core::Texture*> m_boundTextures; // Determiens What is CURRENTLY bound to slot N
        std::unordered_map<Core::Texture*, int> m_batchLookup; // Storres the lookups of textures (Stored per batch/drawCall)

        //Store active viewport
        Dexium::Renderer::Viewport m_activeViewport = {};
        // Store active camera
        Dexium::Core::baseCamera* m_activeCamera = nullptr;
        //Store active shader
        int m_activeShader = 0;
        //Store next texture slot
        GLenum m_activeTexture = 1; // 0 reserved for fallback texture

        //Store vec of passes. Cleared at end of Renderer::flush
        std::vector<RenderPass*> m_renderPasses;
    };


}

#endif //DEXIUM_RENDERER_HPP