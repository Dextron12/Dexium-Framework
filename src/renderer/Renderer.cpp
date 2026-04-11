//
// Created by ethan on 24/2/26.
//

#include <renderer/Command.hpp>
#include <renderer/RenderPass.hpp>
#include <renderer/RenderTarget.hpp>
#include <renderer/viewport.hpp>
#include <renderer/Renderer.hpp>

#include <core/Mesh.hpp>
#include <core/Material.hpp>
#include <core/Transform.h>

#include <core/Error.hpp>

#include "core/Camera.hpp"
#include "core/Texture.hpp"

namespace Dexium::Renderer {

    Renderer::Renderer() {
        // Poll GL for max supported textures for active device
        m_maxTextureSlots = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureSlots); // I believe this polls how many active texture slots can be used within the FRAGMENT shader

        // Resize the cached texture array to support max slots
        m_boundTextures.resize(m_maxTextureSlots, nullptr);
        // Should really be using reserve instead to avoid dummy values. Just check in the flush() that we dont' exceed this alloc when emplacing
        // Actually, resize() creates the values to max slots allowing us to iterate through it m(which is exactly what we do when binding textures to slots), so by using reserve or not populating the vec to maxSize we break slot iterations (Will lead to UB... I've spent too long tracking down a segfault becasue fo this!!)
    }

    int Renderer::pollHW_MaxTexSlots() const {
        return m_maxTextureSlots;
    }

    void Renderer::submit(RenderPass* pass) {
        // Validate the pass state data. No Need to validate any command data, this is done within the pass submission
        // SO we should assume any stored commands are safe to operate on.

        if (!pass) {
            TraceLog(LogLevel::ERROR, "[Renderer]: Invalid pass provided. Check if the pass dtor has been called?");
            return;
        }

        if (pass->camera == nullptr) {
            TraceLog(LogLevel::ERROR, "[Renderer]: RenderPass points to invalid camera");
            return;
        }

        if (pass->renderTarget == nullptr) {
            TraceLog(LogLevel::ERROR, "[Renderer]: RenderPass points to invalid target");
            return;
        }

        // If a target & camera are defined, we can render, so treat all other checks as debug outputs

        // Store pass (Cleared at end of Renderer::flsuh(), when pass executed)
        m_renderPasses.emplace_back(pass);

    }


    void Renderer::flush() {

        // Iterate through the stored passes

        // Should only be reading the pass data, so a iterate for-auto loop will guarantee this
        for (const auto& pass : m_renderPasses) {
            // Internal buffer
            GLenum scrBuffers = 0; // Causes UB if not defined

            // Clear screen if enabled
            if (Utils::hasFlag(pass->plpState.buffers, Utils::BufferTarget::Color)) {
                const auto& col = pass->plpState.passColor;
                glClearColor(col.r(), col.g(), col.b(), col.a());

                scrBuffers |= GL_COLOR_BUFFER_BIT;
            }
            if (Utils::hasFlag(pass->plpState.buffers, Utils::BufferTarget::Depth)) {
                // Depth enabled
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(pass->plpState.depthFunc);

                //Check for depth-writing:
                if (pass->plpState.DepthWrite) {
                    glDepthMask(GL_TRUE);
                } else {
                    glDepthMask(GL_FALSE);
                }

                //Clear depth buffer
                scrBuffers |= GL_DEPTH_BUFFER_BIT;
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            if (pass->plpState.blending) {
                glEnable(GL_BLEND);
                glBlendFunc(pass->plpState.blendMode.src, pass->plpState.blendMode.dst);
            } else {
                glDisable(GL_BLEND);
            }

            // Actually clear the buffers
            glClear(scrBuffers);

            // Update VP to match renderTarget
            if (pass->renderTarget->m_viewport != m_activeViewport) {
                m_activeViewport = pass->renderTarget->m_viewport;

                // Update GL viewport
                const auto& vp = m_activeViewport;
                glViewport(vp.x, vp.y, vp.w, vp.h);
            }

            // Configure the Camera, Projection & View matrices for pass
            if (pass->camera != m_activeCamera) {
                // Swap camera out
                m_activeCamera = pass->camera;
                // Currently not really needed? Proceeding code always uses pass->camera OR pass->VP
            }

            // Refs to Proj & View matrices
            const auto& Projection = pass->camera->getProjectionMatrix(m_activeViewport);
            const auto& View = pass->camera->getViewMatrix();

            // Sort pass commands
            std::sort(pass->m_commands.begin(), pass->m_commands.end(),
                [&state = pass->plpState](const Command& a, const Command& b) {
                    // Sort by shader
                    if (a.material->shader != b.material->shader)
                        return a.material->shader < b.material->shader;

                    // Sort by Z buffer if blending enabled
                    if (state.blending) {
                        // Sort by Z order (Back to front ordering)
                        if (a.transform->position.z != b.transform->position.z)
                            return a.transform->position.z > b.transform->position.z;
                    }

                    // fallback sort: By material
                    return a.material < b.material;
                });

            // clear texture batch lookup
            m_batchLookup.clear();
            m_nextTextureSlot = 1; // 0 is reserved for fallback texture

            // Now iterate over the comms

            for (const auto& cmd : pass->m_commands) {
                // Check if shader program ahs changed
                if (cmd.material->shader->ID != m_activeShader) {
                    // Really should use a proper handle here!!
                    // Update the active shader program, if compiled!
                    if (cmd.material->shader->isCompiled()) {
                        m_activeShader = cmd.material->shader->ID;

                        // Bind it
                        cmd.material->shader->bind();

                        // Set the View & Projection matrices for this pass
                        if (!pass->plpState.Projection_uName.empty()) {
                            cmd.material->shader->setUniform(pass->plpState.Projection_uName.c_str(), Projection);
                        } else {
                            TraceLog(LogLevel::WARNING, "[Renderer]: No uniform name for Projection is configured!\nIf it isn't explicitly ste through the material uniform nothing will render!");
                        }

                        if (!pass->plpState.View_uName.empty()) {
                            cmd.material->shader->setUniform(pass->plpState.View_uName.c_str(), View);
                        } else {
                            TraceLog(LogLevel::WARNING, "[Renderer]: No uniform name for View is configured!");
                        }

                    } else {
                        TraceLog(LogLevel::WARNING, "[Renderer]: Shader({}) hasn't been compiled, yet is trying to be used in a hot render command!\nThis command will fail to output much without a valid shader program", cmd.material->shader->ID);
                    }
                }

                // proceeding code sets MVP + Material uniforms(if material ahs changed)
                // we, could further optimise by checking if the MVP matrices have changed
                // but we can assume that View will change almsot every frame(whenever the camera mvoes)
                // and Model may change frequently if batching, instancing

                // Bind textures
                auto& textures = cmd.material->getTextures();

                for (auto& [samplerName, texture] : textures) {
                    int slot = 0;

                    // Resolve slot for this batch
                    auto it = m_batchLookup.find(texture);
                    if (it != m_batchLookup.end()) {
                        slot = it->second;
                    } else {
                        // Select next available slot and reserve it for this texture
                        if (m_nextTextureSlot >= m_maxTextureSlots) {
                            // In the future, this should be treated as a hard cap to rpevent abuse by end-user
                            m_batchLookup.clear();
                            m_nextTextureSlot = 1;
                            // Above code clears the batched slots when we've hit capacity, now WARN of perf hit
                            TraceLog(LogLevel::WARNING, "[Renderer]: Texture Slots have exceeded limit ({} slots available), {} used!\nBe aware of possible perf hits! Consider using a Texture Atlas!", m_maxTextureSlots, m_maxTextureSlots + m_nextTextureSlot);
                        }

                        // Allocate slot for this texture and move to next slot
                        slot = m_nextTextureSlot++;
                        m_batchLookup[texture] = slot;
                    }

                    // Bind Texture to slot, if not the active texture
                    if (m_boundTextures[slot] != texture) {
                        glActiveTexture(GL_TEXTURE0 + slot);
                        glBindTexture(GL_TEXTURE_2D, texture->texID);

                        m_boundTextures[slot] = texture;
                    }

                    // Set shader sampler uniform to slot
                    cmd.material->shader->setUniform(samplerName, slot); // Use raw shader.setUniform here as its state doesn't persist unlike Material uniform setting does!!
                }

                // Set Model Matrix (From cmd Transform data)
                if (!pass->plpState.Model_uName.empty()) {
                    cmd.material->shader->setUniform(pass->plpState.Model_uName.c_str(), cmd.transform->ModelMatrix());
                } else {
                    TraceLog(LogLevel::WARNING, "[Renderer]: No uniform name for Model is configured!");
                }

                // Now set material property uniforms (If, material has changed)
                if (m_activeMaterial != cmd.material) {
                    for (const auto& pair : cmd.material->getUniforms()) {
                        const auto& uniformName = pair.first;
                        const auto& variant = pair.second;

                        // Visit the raw varient value:
                        std::visit([shader = cmd.material->shader, name = uniformName](auto&& value) {
                            shader->setUniform(name, value);
                        }, variant);
                    }
                }

                // Begin drawing

                // Bind Mesh VAO
                glBindVertexArray(cmd.mesh->VAO);

                // THe mystical drawing sauce that took me ~2.5kLOC to get something drawn...

                if (cmd.mesh->EBO != 0) {
                    // using indices for drawing
                    glDrawElements(cmd.mesh->drawMode, cmd.mesh->indexCount, GL_UNSIGNED_INT, 0);
                } else {
                    // Vertex drawing mode
                    glDrawArrays(cmd.mesh->drawMode, 0, cmd.mesh->vertexCount);
                }
            }

            // Force clear the commands from each pass (to rpevent stale state)
            pass->clearCommands();
        }

        glDisable(GL_BLEND);

        // Clear stored passes
        m_renderPasses.clear(); // Avoids stale states



    }
}


/*
#include <../../include/renderer/Renderer.hpp>

// Finish forward declares 9Avoids cross incldue dependsencies)

#include <core/Mesh.hpp>
#include <core/Material.hpp>
#include <core/Transform.h>

#include "core/Texture.hpp"

#include <core/windowContext.hpp>

namespace Dexium::Core {
    void RenderPass::storeCommand(Mesh* mesh, Material* material, Transform* transform) {
        auto comm = RenderCommand{mesh, material, transform};
        storeCommand(comm);
    }

    void RenderPass::storeCommand(const RenderCommand& command) {

        // Mesh valdiation
        if (!command.mesh) return;

        if (!command.material) return;

        if (!command.transform) return;

        commands.push_back(command);
    }


    RenderCommand::RenderCommand(Mesh* mesh_, Material* material_, Transform* transform_)
            : mesh(mesh_), material(material_), transform(transform_) {}

    Renderer::Renderer(WindowContext *windowCtx) {

        //Poll the amxiumum supported textures
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureSlots);

        //Resize the m_texcfache array to amtch MAxTextures
        m_boundTextures.resize(m_maxTextureSlots, nullptr);
    }

    int Renderer::pollHW_MaxTexSlots() const{
        return m_maxTextureSlots;
    }

    void Renderer::flush() {

    }


    /*void Renderer::submit(const RenderCommand &cmd) {
        //Check for unset ptr's
        // cannot store a renderComm that is nto fully created!
        // It means we waste CPU sorting time and require more stingent checks on each operation

        // Validate renderTarget
        if (cmd.target == nullptr) return;

        //Mesh valdiation
        if (cmd.mesh == nullptr) return;

        //Material (&Shader) validation
        if (cmd.material == nullptr) return;
        if (cmd.material->shader == nullptr) return;
        //Check compile status
        if (!cmd.material->shader->isCompiled()) return;

        //Transform
        if (cmd.transform == nullptr) return;

        if (cmd.pass == RenderPass::Opaque) {
            m_OpaqueComms.push_back(cmd);
        } else {
            // Transparent comm
            m_TransparentComms.push_back(cmd);
        }
    }

    void Renderer::submit(const RenderPass& pass) {
        //Validate pass state
        if (!pass.camera) return;
        if (!pass.target) return;
        if (pass.commands.empty()) return;
    }

    /*void Renderer::setClearColor(Color color, bufferTargets buffs) {
        stateCol = color;

        if (buffs != bufferTargets::None) {
            clearBuffers(buffs);
        }
    }

    void Renderer::clearBuffers(bufferTargets targets) {
        //Renderer::Flush clears the actual GL buffers.
        if (targets == bufferTargets::None) {
            bufferMasks = 0;
        }

        if (hasFlag(targets, bufferTargets::Color)) bufferMasks |= GL_COLOR_BUFFER_BIT;

        if (hasFlag(targets, bufferTargets::Depth)) bufferMasks |= GL_DEPTH_BUFFER_BIT;

        if (hasFlag(targets, bufferTargets::Stencil)) bufferMasks |= GL_STENCIL_BUFFER_BIT;

    }













}
*/