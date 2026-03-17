//
// Created by ethan on 24/2/26.
//

#include <core/Renderer.hpp>

// Finish forward declares 9Avoids cross incldue dependsencies)

#include <core/Mesh.hpp>
#include <core/Material.hpp>
#include <core/Transform.h>

#include "core/Texture.hpp"

#include <core/windowContext.hpp>

namespace Dexium::Core {


    RenderCommand::RenderCommand(RenderState::RenderTarget* target_,
        Mesh* mesh_, Material* material_, Transform* transform_, RenderPass pass_)
            : target(target_), pass(pass_), mesh(mesh_), material(material_), transform(transform_) {}

    Renderer::Renderer(WindowContext *windowCtx)
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
        //Renderer::Flush clears the actual GL buffers.
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
        // Sort Opaque layer
        std::sort(m_OpaqueComms.begin(), m_OpaqueComms.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                if (a.target != b.target)
                    return a.target < b.target;

                if (a.material->shader < b.material->shader)
                    return a.material->shader < b.material->shader;

                return a.material < b.material;
            });

        // Sort Transparent layer
        std::sort(m_TransparentComms.begin(), m_TransparentComms.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                if (a.target != b.target)
                    return a.target < b.target;

                if (a.material->shader < b.material->shader)
                    return a.material->shader < b.material->shader;

                // Z ordering (Back to front ordering)
                return a.transform->position.z > b.transform->position.z;
            });

        // render Opaque
        for (const auto& com : m_OpaqueComms) {
            //Check if the viewport has changed
            if (com.target->m_viewport != m_activeViewport) {
                m_activeViewport = com.target->m_viewport;
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

            // Set MVP (Model, View, Projection matrices)
            //Currently no Camera system exists and no View matrix.
            // However, a Camera could bring its own Proj + View & we already have Model from the Transform attrib
            //com.material->shader->setUniform("U_Projection", com.target->m_viewport.)

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

        // Render transparent
        for (const auto& com : m_TransparentComms) {
            //Check if the viewport ahs changed
            if (com.target->m_viewport != m_activeViewport) {
                m_activeViewport = com.target->m_viewport;
                glViewport(m_activeViewport.x, m_activeViewport.y, m_activeViewport.w, m_activeViewport.h);
            }

            //Check for shader changes
            if (m_activeShader != com.material->shader->ID) {
                m_activeShader = com.material->shader->ID;
                com.material->shader->bind();
            }

            //Clear batch lookup
            m_batchLookup.clear();
            m_nextTextureSlot = 1; // 0 reserved for fallback

            //handle texture binding
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
                        // Its important to understand that GL texture units(what ive called slots here) don't need to be persited. Hence why its low impact on perf to just flush the cache when full.
                        // Perhaps a debug macro and TraceLog(WARNING) can be slapped here to warn game devs, that they are thrashing through alotted slots frequently and this likely will have a perf impact
                    }
                    slot = m_nextTextureSlot++;
                    m_batchLookup[texture] = slot;
                }

                //Bidn only if needed
                if (m_boundTextures[slot] != texture) {
                    glActiveTexture(GL_TEXTURE0 + slot);
                    glBindTexture(GL_TEXTURE_2D, texture->texID);

                    m_boundTextures[slot] = texture;
                }

                //Set shader sampler uniform to slot
                com.material->shader->setUniform(samplerName, slot); // Use raw shader.setUniform here as its state doesnt eprsist, unlike Material uniform setting
            }

            // Now set material uniforms
            // !! POTENTIAL BUG !!
            /*
             *It should be documented that if the end-user explicitly sets the texture smapler via Material
             *it will override the work the renderer has doen above. For now I plan to leave this as is
             *in case power users want finer control over slots
             *but if novice users are to do this they likely will break renderering, hence shopuld be documented
             */

            // Use of variant makes this particularly painful to access the stored uniforms
            for (const auto& pair : com.material->getUniforms()) {
                const auto& uniformName = pair.first;
                const auto& variant = pair.second;

                //Now visti varaint value (If compiled in CXX20, it allows for parsing dynamic values into lambdas, so a neater for loop could be used)
                std::visit([&](auto&& value) {
                    com.material->shader->setUniform(uniformName, value);
                }, variant);
            }

            // Begin drawing (Need to look in to how I can better combine the Opaque + Transparent for drawing)

            // Enable alpha testing
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            //Bind VAO
            glBindVertexArray(com.mesh->VAO);
            glDepthMask(GL_FALSE); // Dont write depth

            // Don't forget the apple sauce!!

            if (com.mesh->EBO != 0) {
                // Using an EBO for rendering
                glDrawElements(com.mesh->drawMode, com.mesh->indexCount, GL_UNSIGNED_INT, nullptr);
            } else {
                // Vertex drawing mode
                glDrawArrays(com.mesh->drawMode, 0, com.mesh->vertexCount);
            }

            // Disable alpha testing
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE); // rEANBLE IT FOR oPAQUE pass
        }
        //Clear TransparentComms queue
        m_TransparentComms.clear();

    }






}
