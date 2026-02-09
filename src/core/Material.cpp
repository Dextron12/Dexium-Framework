//
// Created by Dextron12 on 16/12/25.
//

#include <core/Material.hpp>

namespace Dexium::Core {

    void Material::remUniform(const std::string &name) {
        if (uniforms.find(name) != uniforms.end()) {
            uniforms.erase(name);
            TraceLog(ErrorType::STATUS, "[Material]: Removed the uniform '{}' from material", name);
        }
    }

    void Material::clearUniforms() {
        uniforms.clear();
    }

    const std::unordered_map<std::string, Material::UniformValue> &Material::getUniforms() const {
        return uniforms;
    }



}