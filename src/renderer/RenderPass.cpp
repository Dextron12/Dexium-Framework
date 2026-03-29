//
// Created by ethan on 29/3/26.
//

#include <renderer/RenderPass.hpp>
#include <renderer/Command.hpp>

#include <core/Material.hpp>
#include <core/Mesh.hpp>
#include <core/Transform.h>
#include <core/Camera.hpp>

#include <core/Error.hpp>

namespace Dexium::Renderer {
    RenderPass::RenderPass(RenderTarget* renderTarget, Core::baseCamera* camera)
        : renderTarget(renderTarget), camera(camera) {}

    void RenderPass::storeCommand(const Command& cmd) {
        storeCommand(cmd.mesh, cmd.material, cmd.transform);
    }

    void RenderPass::clearCommands() {
        if (m_commands.size() > 0) {
            m_commands.clear();
        }
    }

    void RenderPass::setClearColor(Colour& color) {
        auto& state = plpState;

        state.passColor = color;

        //Ensure the buffer is configured to clear color
        if (!Utils::hasFlag(state.buffers, Utils::BufferTarget::Color))
            state.buffers |= Utils::BufferTarget::Color;
    }

    void RenderPass::storeCommand(Core::Mesh* mesh, Core::Material* material, Core::Transform* transform) {
        //Validate cmd
        if (mesh == nullptr) {
            TraceLog(LogLevel::DEBUG, "[RenderCommand]: Mesh ptr is invalid. Cannot store command");
            return;
        }
        if (material == nullptr) {
            TraceLog(LogLevel::DEBUG, "[RenderCommand]: Material ptr is invalid. Cannot store command");
            return;
        }
        if (transform == nullptr) {
            TraceLog(LogLevel::DEBUG, "[RenderCommand]: Transform ptr is invalid. Cannot store command");
            return;
        }

        // Validate Mesh internals:
        if (mesh->VAO < 0 || mesh->VBO < 0 || (mesh->indexCount > 0 && mesh->EBO < 0)) {
            TraceLog(LogLevel::WARNING, "[RenderCommand]: Mesh data appears out-of-range. It may NOT render correctly or at all.");
        }

        // Ensure Material points to a valid shader program
        if (material->shader != nullptr) {
            // Ensure shader is compiled
            if (material->shader->isCompiled() == false) {
                TraceLog(LogLevel::DEBUG, "[RenderCommand]: THe associated shader to this commands material hasn't been compiled!");
                return;
            }

            // Ensure shader has a valid ID
            if (material->shader->ID < 1) {
                TraceLog(LogLevel::DEBUG, "[RenderCommand]: The materials associated shader ID({}) is invalid", material->shader->ID);
                return;
            }
        } else {
            // Catch shader nullptr
            return;
        }

        // Store command
        m_commands.emplace_back(mesh, material, transform);
    }
}

