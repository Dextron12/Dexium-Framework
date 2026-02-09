//
// Created by Dextron12 on 15/12/25.
//

#ifndef DEXIUM_MATERIAL_HPP
#define DEXIUM_MATERIAL_HPP

// Used for hashed string ID's
#include <entt/core/hashed_string.hpp>

// Core headers
#include <core/Shader.hpp>

#include <variant>

namespace Dexium::Core {

    class Material {
    public:
        using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

        template<typename T, typename Variant>
        struct variant_contains;

        template<typename T, typename... Ts>
        struct variant_contains<T, std::variant<Ts...>> : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};

        entt::id_type ID;

        // Shader program
        Shader* shader = nullptr;

        template<typename T>
        void setUniform(const std::string& name, const T& value) {
            // Check if the value is contained within the variant
            static_assert(variant_contains<T, UniformValue>::value, "Unsupported uniform type");

            auto [it, inserted] = uniforms.insert_or_assign(name, value);

            if (!inserted) {
                // WARN: Overriting existing uniform
                TraceLog(ErrorType::WARNING, "[Material]: Overwriting uniform '{}' with a new value", name.c_str());
            }

            // Make/Update entry
            uniforms[name] = value;
        };

        Material() = default;

        void remUniform(const std::string& name);
        void clearUniforms();

        // Get ref of uniform map
        const std::unordered_map<std::string, UniformValue>& getUniforms() const;

        // We purposely DON'T provide a bind() as the Render will internally handle this
        // But perhaps this funciton should provide one that will allow it to work independent of a renderer
        // to allign material with the phiolosphy of the framework

    private:
        std::unordered_map<std::string, UniformValue> uniforms;


    };
}

#endif //DEXIUM_MATERIAL_HPP