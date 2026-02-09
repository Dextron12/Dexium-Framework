//
// Created by ethan on 11/12/25.
//

#ifndef DEXIUM_RENDERER_H
#define DEXIUM_RENDERER_H

#include <core/Mesh.hpp>
#include <core/Material.hpp>
#include <core/Transform.h>
#include <core/viewport.hpp>
#include <core/windowContext.hpp>

#include <glad/gl.h>

#include "Material.hpp"

namespace Dexium::Globals {
    // Defines the GL buffers used by the global state (E.g GL_COLOR_BUFFER_BIT)
    extern GLbitfield GwinMasks;
}

namespace Dexium::Core {

    struct RenderCommand {
        Mesh* mesh = nullptr;
        Material* material = nullptr;

        Transform* transform = nullptr;

        //Viewport* viewport = nullptr;
        int viewportID = 0; // ) is the default constructed viewport
    };

    class Renderer {
    public:
        // The initaliser requires a windowCtx has it will create the default viewport of the window size and mimic its size
        // In the future, more renderer args may be added. For instance the GLbitfield flags might be taken and used to configure the state of the renderer
        Renderer(windowContext* window) : m_windowCtx(window) {};
        // each RenderCommand is a instruction for the renderer to sort then render the object

        // Move constructor
        // !!!IMPORTANT: Renderer is non-copyable, but movable. std::move is USED on GameLayer when moving the ref into the engine memory. So in order to construct a locally accessible renderer object for a GameLayer it must be movable
        Renderer(Renderer&& other) noexcept
        : m_windowCtx(other.m_windowCtx)
        , commands(other.commands)
        , m_viewports(other.m_viewports)
        , m_activeShaderID(other.m_activeShaderID) {
            other.m_windowCtx = nullptr; // leave moved-from safe
        }

        // Move assignment
        Renderer& operator=(Renderer&& other) noexcept {
            if (this != &other) {
                m_windowCtx = other.m_windowCtx;
                commands = std::move(other.commands);
                m_viewports = std::move(other.m_viewports);
                m_activeShaderID = other.m_activeShaderID;

                other.m_windowCtx = nullptr;
            }
            return *this;
        }

        // Prevent copies
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        // Sorts by shader -> texture
        void submit(const RenderCommand& command);

        // Actually renders the provided commands then clears them. SHould be called each frame
        void flush();

        // Create a new viewport of a specific size(relative to the renderer)
        int createViewport(int x, int y, int designWidth, int designHeight);

        // Remove a viewport(even the default one)
        void destroyViewport(int viewportID);

        // Fetch a specific viewport as a ref
        const Viewport& getViewport(int viewportID);

    private:
        windowContext* m_windowCtx;
        std::vector<RenderCommand> commands;

        //Stores all configured viewports for the renderer
        std::vector<Viewport> m_viewports;

        GLuint m_activeShaderID = 0;

        // Remove default construction of renderer
        Renderer() = delete;

    };
}

#endif //DEXIUM_RENDERER_H