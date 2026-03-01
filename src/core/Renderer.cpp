//
// Created by ethan on 24/2/26.
//

#include <core/Renderer.hpp>

// Finish forward declares 9Avoids cross incldue dependsencies)

#include <core/Mesh.hpp>
#include <core/Material.hpp>
#include <core/Transform.h>

#include <core/viewport.hpp>
#include <core/windowContext.hpp>

#include "core/Texture.hpp"

namespace Dexium::Core {

    RenderTarget::RenderTarget(Viewport* viewport_)
        : viewport(*viewport_) {}

    RenderCommand::RenderCommand(RenderTarget* target_,
        Mesh* mesh_, Material* material_, Transform* transform_, RenderPass pass_)
            : target(target_), pass(pass_), mesh(mesh_), material(material_), transform(transform_) {}

    Renderer::Renderer(windowContext *windowCtx)
        : m_winCtx(windowCtx) {

        m_bufferTargets = bufferTargets::None;
        bufferMasks = 0;

        //Poll the amxiumum supported textures
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureSlots);

        //Resize the m_texcfache array to amtch MAxTextures
        m_boundTextures.resize(m_maxTextureSlots, nullptr);
    }

    int Renderer::pollHW_MaxTexSlots() const{
        return m_maxTextureSlots;
    }


    void Renderer::submit(const RenderCommand &cmd) {
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

    void Renderer::setClearColor(Color color, bufferTargets buffs) {
        stateCol = color;

        if (buffs != bufferTargets::None) {
            clearBuffers(buffs);
        }
    }

    void Renderer::clearBuffers(bufferTargets targets) {
        if (targets == bufferTargets::None) {
            bufferMasks = 0;
        }

        if (hasFlag(targets, bufferTargets::Color)) bufferMasks |= GL_COLOR_BUFFER_BIT;

        if (hasFlag(targets, bufferTargets::Depth)) bufferMasks |= GL_DEPTH_BUFFER_BIT;

        if (hasFlag(targets, bufferTargets::Stencil)) bufferMasks |= GL_STENCIL_BUFFER_BIT;

    }






    void Renderer::flush() {
        //State level clear and color
        glClearColor(stateCol.r() / 255, stateCol.g() / 255, stateCol.b() / 255, stateCol.a() / 255);
        glClear(bufferMasks);

        /*
         *RenderTarget (Viewport, FBO's) (Perhaps need to sort by FBO independtly in the future)
         *Shader
         *Material
         */

        std::sort(m_OpaqueComms.begin(), m_OpaqueComms.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                if (a.target != b.target)
                    return a.target < b.target;

                if (a.material->shader < b.material->shader)
                    return a.target < b.target;

                return a.material < b.material;
            });

        // render Opaque
        for (const auto& com : m_OpaqueComms) {
            //Check if the viewport has changed
            if (com.target->viewport != m_activeViewport) {
                m_activeViewport = com.target->viewport;
                glViewport(m_activeViewport.x, m_activeViewport.y, m_activeViewport.w, m_activeViewport.h);
            }

            // Check for shader changes
            if (m_activeShader != com.material->shader->ID) {
                m_activeShader = com.material->shader->ID;
                com.material->shader->bind();
            }

            //Clear batch lookup
            m_batchLookup.clear();
            m_nextTextureSlot = 1; // ) reserved for fallback

            // Handle texture binding
            auto& textures = com.material->getTextures();

            for (auto& [samplerName, texture] : textures) {
                int slot;

                //resolve slot for this batch
                auto it = m_batchLookup.find(texture);
                if (it != m_batchLookup.end()) {
                    slot = it->second;
                } else {
                    if (m_nextTextureSlot >= m_maxTextureSlots) {
                        m_batchLookup.clear();
                        m_nextTextureSlot = 1;
                    }
                    slot = m_nextTextureSlot++;
                    m_batchLookup[texture] = slot;
                }

                //Bind only if needed
                if (m_boundTextures[slot] != texture) {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    glBindTexture(GL_TEXTURE_2D, texture->texID);

                    m_boundTextures[slot] = texture;
                }

                // Set shader sampler uniform to SLOT
                com.material->shader->setUniform(samplerName, slot); // Use raw shader.setUniform here as its state doesnt persist, unlike Material uniform setting
            }

            // Now set material uniforms
            for (const auto& pair : com.material->getUniforms()) {
                const auto& uniformName = pair.first;
                const auto& variant = pair.second;

                std::visit([&](auto&& value) {
                    com.material->shader->setUniform(uniformName, value);
                }, variant);
            }

            //Begin drawing


            //bind VAO (recover mesh state)
            glBindVertexArray(com.mesh->VAO);

            // THe drawing sauce

            if (com.mesh->EBO != 0) {
                // Using indices for drawing
                glDrawElements(com.mesh->drawMode, com.mesh->indexCount, GL_UNSIGNED_INT, nullptr);
            } else {
                // Vertex drawing mode
                glDrawArrays(com.mesh->drawMode, 0, com.mesh->vertexCount);
            }
        }
        //Clear the OpaqueComms queue
        m_OpaqueComms.clear();

    }






}
