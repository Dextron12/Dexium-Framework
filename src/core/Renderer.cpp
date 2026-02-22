//
// Created by ethan on 11/12/25.
//

#include <algorithm>
#include <core/Renderer.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> // Used to print glm::mat

namespace Dexium::Globals {
    GLbitfield GwinMasks = 0;
}

namespace Dexium::Core {

    void Renderer::submit(const RenderCommand& command) {
        commands.push_back(command);
    }

    // Actions all submitted render commands
    void Renderer::flush() {
        // Firsta ction engine level render refreshes

        // Action Buffers:
        /*
         * Instead of manual glClearColour + glClear as this assumes the end-user knows basic GL
         * The engine instead submits a buffer to the global var Globals::GwinMasks(GLbitfield)
         * and the enigne then clears those buffers and resets the var so theres no stale buffer state between frames
         * For instance, clearColour(a non-namespaced func) makes a call to glClearColor with its defined colour, then checks if GL_COLOR_BUFFER_BIT is set for clearing in the netx frame(if it exists inside Globals::GwinMasks
         * and sets it for clearing. This way, the end-user doesnt need to worry about clearing buffers at all and buffers aren't enabled once for an entire applications lifetime
         */
        glClear(Globals::GwinMasks);

        // Loop through renderComms looking for scene level updates
        // E.g: viewport/window changes
        for (auto& comm : commands) {
            //Check viewport state and update
            if (comm.viewportID < 0) {
                TraceLog(LogLevel::ERROR, "[Renderer]: No viewport defined for the specified renderCommand");
                continue;
            }
            /*if (comm.viewport->isDirty()) {
                //update viewport, viewport automatically scales itself on globalr esize signal
                glViewport(comm.viewport->x, comm.viewport->y, comm.viewport->width, comm.viewport->height);
                // Mark clean
                comm.viewport->dirty = false; // PROTECTED VAR -> friend in use
            }*/
        }

        // Check if there is anything to draw
        if (commands.size() < 1) return; // Nothing to draw

        // Sort RenderComamnds in order of Shader
        std::sort(commands.begin(), commands.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                /*if (a.material.shader->ID != b.material.shader->ID) {
                    // Return the lower shaderID first
                    return a.material.shader->ID < b.material.shader->ID;
                }*/
                // Add other sort filters here

                // default return
                return a.material->shader->ID < b.material->shader->ID;
            });

        // Now draw
        for (auto& comm : commands) {
            // validate ptr's
            if (!comm.mesh) {
                TraceLog(LogLevel::ERROR, "[Renderer]: RenderCommand contains an invalid ptr to Mesh. Ignoring the command!");
                continue;
            }
            if (!comm.material) {
                TraceLog(LogLevel::ERROR, "[Renderer]: RenderCommand contains an invalid ptr to Material. Ignoring the command!");
                continue;
            }
            if (!comm.transform) {
                TraceLog(LogLevel::ERROR, "[Renderer]: RenderCommand contains an invalid ptr to Transform. Ignoring the command!");
                continue;
            }

            //fmt::print(stderr, fg(fmt::color::blue), comm.)

            //fmt::print(stderr, fg(fmt::color::green), "{}\n", glm::to_string(comm.transform->ModelMatrix()));
            //comm.material->shader->setUniform("model", comm.transform->ModelMatrix());

            if (comm.viewportID < 0) {
                // Need to decide, if we should error and ingore the current comm if no viewport is provided
                // If no viewport, GL jsut uses the defautl viewport created at window creation. So resizing wont apply correctly
                TraceLog(LogLevel::WARNING, "[Renderer]: No viewport has been specified for the render comm. Rendering will fallback to default viewport");
                // Set comm->viewport to default(0)
                comm.viewportID = 0;
            }

            if (comm.material->shader->ID != m_activeShaderID) {
                // Shader request to change programs
                comm.material->shader->bind(); // There are already MINIMIAL safegaurds in palce to ensure that a shader that hasnt been compiled yet, cant bind (A warning is output)
                // Update activeSahder
                m_activeShaderID = comm.material->shader->ID;
            }

            // Eventually set Textures here (Only sets one texture, perhaps Rebnderer polls GL for max supported textures
            // (... then Material inialiser takes renderer as param ptr and can access max supportec textures + recommended per amt max texture limit
            // (... then back in this bit of code, we can bind and activate to each specified texture unit, perhaps use a texture unit recycler too
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 1);
            comm.material->shader->setUniform("tex", 1);

            // Set shader uniforms (per material)
            for (auto& pair : comm.material->getUniforms()) {
                const auto& name = pair.first;
                const auto& variant = pair.second;
                std::visit([&](auto&& value) {
                    comm.material->shader->setUniform(name, value);
                }, variant);
            }

            //Bind VAO and begin drawing
            glBindVertexArray(comm.mesh->VAO);

            if (comm.mesh->EBO != 0) {
                // EBO in use, draw by elements
                glDrawElements(comm.mesh->drawMode, comm.mesh->indexCount, GL_UNSIGNED_INT, nullptr);
            } else {
                glDrawArrays(comm.mesh->drawMode, 0, comm.mesh->vertexCount);
            }


        }
        // Renderering complete, clear the commands
        commands.clear();
    }

    int Renderer::createViewport(int x, int y, int designWidth, int designHeight) {
        // Check for invalid windowCtx
        if (m_windowCtx == nullptr) {
            TraceLog(LogLevel::FATAL, "[Renderer]: Context is out of scope! Either an invalid context were provided to the renderer on creation, or it has been manually deleted/freed");
        }
        const windowContext& winCtx = *m_windowCtx;
        m_viewports.emplace_back(winCtx, x, y, designWidth, designHeight);

        // Get size and return ID
        auto count = m_viewports.size();
        if (count == 1) {
            TraceLog(LogLevel::STATUS, "[Renderer]: Default Viewport created: Defined as [X: {}, Y: {}, W: {}, H: {}", x, y, designWidth, designHeight);
        }

        return count;
    }



}