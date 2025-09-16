//
// Created by Dextron12 on 14/09/2025.
//

#include "Primitives.hpp"

#include <glm/vec4.hpp>

#include "Colour.hpp"
#include "Shader.hpp"
#include "AssetManager.hpp"

void Primitives::FilledRect(glm::vec4 pos, Colour colour, const Shader& shader) {

    Transform model;
    model.position = glm::vec3(pos.x, pos.y, 0.0f);
    model.scale = glm::vec3(pos.z, pos.w, 1.0f);

    /*
    Primitives now use the same 2D shader as Sprites etc!
    This means that it needs to sue a texture, a default white texture is auto-genrated udner the ID
    "__whiteTexture", this should be used to render primitives
    IMPORTANT: A white texture is used because (1,1,1,1) * primitive colour = primitiveColour
     */

    // Configure shader params:
    shader.setMat4("model", model.toModelMatrix());
    shader.setVec4("uColor", colour.normalized());


    // Configure default white texture for usage
    auto& assets = AssetManager::getInstance();
    auto tex = assets.use<Texture2D>("__whiteTexture");
    if (!tex){ TraceLog(LOG_ERROR, "Failed to fetch primitives texture"); return;}

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->ID);
    shader.setInt("texture1", 0);

    getUnitRect()->render();

    // Unbind Texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Primitives::Rect(glm::vec4 pos, Colour colour, const Shader &shader, int thickness) {
    // pos = x, y, width, height
    // thickness = border thickness in pixels/units

    // Configure default white texture
    auto& assets = AssetManager::getInstance();
    auto tex = assets.use<Texture2D>("__whiteTexture");
    if (!tex) { TraceLog(LOG_ERROR, "Failed to fetch primitives texture"); return; }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->ID);
    shader.setInt("texture1", 0);

    // Enable stencil
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);

    // Set color
    shader.setVec4("uColor", colour.normalized());

    // Outer rect model
    Transform outerModel;
    outerModel.position = glm::vec3(pos.x + pos.z / 2.0f, pos.y + pos.w / 2.0f, 0.0f); // center of quad
    outerModel.scale = glm::vec3(pos.z, pos.w, 1.0f);

    // Inner rect model
    float innerW = std::max(pos.z - thickness * 2.0f, 1.0f);
    float innerH = std::max(pos.w - thickness * 2.0f, 1.0f);
    Transform innerModel;
    innerModel.position = glm::vec3(pos.x + thickness + innerW / 2.0f,
                                    pos.y + thickness + innerH / 2.0f,
                                    0.0f);
    innerModel.scale = glm::vec3(innerW, innerH, 1.0f);

    // ---- STENCIL STEPS ----

    // 1. Draw outer rect -> stencil = 1 (only write stencil, no color)
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    shader.setMat4("model", outerModel.toModelMatrix());
    getUnitRect()->render();

    // 2. Draw inner rect -> stencil = 0 (cut out)
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    shader.setMat4("model", innerModel.toModelMatrix());
    getUnitRect()->render();

    // 3. Draw border -> stencil != 0, now write color
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    shader.setMat4("model", outerModel.toModelMatrix());
    getUnitRect()->render();

    // Disable stencil
    glDisable(GL_STENCIL_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
}


