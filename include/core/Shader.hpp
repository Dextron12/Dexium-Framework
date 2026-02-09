//
// Derived from old project by Dextron12 on 03/12/25
//

#ifndef DEXIUM_SHADER_HPP
#define DEXIUM_SHADER_HPP

#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <core/Error.h>

namespace Dexium::Core {

    class Shader {
    public:
        unsigned int ID; // The GL Shaderprogram id

        Shader() = default;
        Shader(const std::string& vertex, const std::string& fragment, bool areFiles = true);

        void compile();

        void bind();

        template<typename T>
        void setUniform(const std::string& name, const T& value) {
            GLint loc;
            if (uniformCache.find(name) != uniformCache.end()) {
                loc = uniformCache[name];
            } else {
                // Not yet cached
                loc = glGetUniformLocation(ID, name.c_str());
                uniformCache[name] = loc;
            }
            // Check if the location of the uniform exists (may be optimized out)
            if (loc == -1) {
                TraceLog(ErrorType::WARNING, "[Shader]: Uniform '{}' not found in shader", name.c_str());
                return;
            }
            // Set the uniform for its type
            if constexpr (std::is_same_v<T, float>) {
                //float
                glUniform1f(loc, value);
            }else if constexpr (std::is_same_v<T, int>) {
                //int
                glUniform1i(loc, value);
            } else if constexpr (std::is_same_v<T, bool>) {
                //bool
                glUniform1i(loc, value);
            } else if constexpr (std::is_same_v<T, glm::vec2>) {
                //vec2
                glUniform2fv(loc, 1, glm::value_ptr(value));
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                glUniform3fv(loc, 1, glm::value_ptr(value));
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                glUniform4fv(loc, 1, glm::value_ptr(value));
            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
            } else {
                TraceLog(ErrorType::ERROR, "[Shader]: Unsupported uniform value requested to be set");
            }
        }

    private:
        std::string vertexCode, fragmentCode;

        std::unordered_map<std::string, GLint> uniformCache; // Per program uniform cache for lookups (no need for continiously polling locations)
    };


    namespace Shaders {
        namespace PREBUILT_2D {
            extern const std::string SHADER_2D_VERTEX;
            extern const std::string SHADER_2D_FRAGMENT;
        }

        Shader generateDefault2DShader();
    }

}

#endif //DEXIUM_SHADER_HPP