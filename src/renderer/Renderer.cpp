//
// Created by ethan on 24/2/26.
//


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