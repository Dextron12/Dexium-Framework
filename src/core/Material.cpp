//
// Created by Dextron12 on 16/12/25.
//

#include <core/Material.hpp>

#include <core/Texture.hpp>

namespace Dexium::Core {

    void Material::remUniform(const std::string &name) {
        if (uniforms.find(name) != uniforms.end()) {
            uniforms.erase(name);
            TraceLog(LogLevel::STATUS, "[Material]: Removed the uniform '{}' from material", name);
        }
    }

    void Material::clearUniforms() {
        uniforms.clear();
    }

    const std::unordered_map<std::string, Material::UniformValue> &Material::getUniforms() const {
        return uniforms;
    }

    void Material::setTexture(const std::string &uniformName, Texture *texPtr) {
        //Check if Texture already stored
        auto key = textures.find(uniformName);
        if (key != textures.end()) {
            //Does ptr match:
            if (texPtr == key->second) {
                TraceLog(LogLevel::WARNING, "[Material][TextureAssignment]: Material is already storing texture: {}\nNo need to set texture over again", texPtr->texID);
                //Early exit
            } else {
                //Texture ptr has changed
                TraceLog(LogLevel::STATUS, "[Material][TextureAssignment]: Texture bound to uniform {} is being replaced by new texture: {}", uniformName, texPtr->texID);
                key->second = texPtr;
            }
        } else {
            // Store texture
            textures[uniformName] = texPtr;
        }
    }

    Texture *Material::getTexture(const std::string &uniformName) {
        auto key = textures.find(uniformName);
        if (key != textures.end()) {
            return key->second;
        }

        //ELSE
        TraceLog(LogLevel::ERROR, "[Material][Texture]: No such texture uniform '{}' is stored", uniformName);
        return nullptr;
    }

    const std::unordered_map<std::string, Texture*>& Material::getTextures() const {
        return textures;
    }





}
