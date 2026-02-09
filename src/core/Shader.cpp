//
// Created by Dextron12 on 3/12/25.
//

#include <core/Shader.hpp>
#include <core/Error.h>
#include <core/VFS.hpp>

#include <fstream>
#include <sstream>

namespace Dexium::Core {
    Shader::Shader(const std::string &vertex, const std::string &fragment, bool areFiles) {

        if (!areFiles) {
            // data is likely raw sources
            vertexCode = vertex;
            fragmentCode = fragment;
        } else {
            // data provided are likely paths, Check if they are rel or abs
            auto vPath = VFS::resolve(vertex);
            if (vPath.empty()) {
                // Path failed to resolve, It cannot be loaded
                TraceLog(ErrorType::ERROR, "[Shader][Vertex]: Cannot load a vertex comp from '{}', it is invalid", vertex);
                return;
            }

            auto fPath = VFS::resolve(fragment);
            if (fPath.empty()) {
                // Path failed to resolve, it cannot be laoded
                TraceLog(ErrorType::ERROR, "[Shader][Vertex]: Cannot load a fragment comp from '{}', it is invalid", fragment);
                return;
            }

            std::string vertexPath = vPath.string();
            std::string fragmentPath = fPath.string();



            if (!std::filesystem::exists(vertexPath)) {
                TraceLog(ErrorType::WARNING, "[Shader][Vertex]: The vertex path '{}' is invalid", vertexPath);
            }
            if (!std::filesystem::exists(fragmentPath)) {
                TraceLog(ErrorType::WARNING, "[Shader][Fragment]: The fragment path '{}' is invalid", fragmentPath);
            }

            // Attempt to read code from files
            std::ifstream vShaderFile, fShaderFile;
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            try {
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;

                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();

                // Close files
                vShaderFile.close();
                fShaderFile.close();

                // Convert streams to strings
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            } catch (std::ifstream::failure e) {
                TraceLog(ErrorType::ERROR, "[Shader][File Read]: Failed to read streams from Vertex: '{}' and Fragment: '{}', Reason: {}", vertexPath, fragmentPath, e.what());
            }
        }
    }

    void Shader::compile() {
        if (vertexCode.empty()) {
            TraceLog(ErrorType::ERROR, "[Shader][Vertex]: Vertex buffer is empty, cannot compile shader with no vertex");
            return;
        }
        if (fragmentCode.empty()) {
            TraceLog(ErrorType::ERROR, "[Shader][Fragment]: Fragment buffer is empty, cannot compile shader with no fragment");
            return;
        }

        // Clear the uniform cache (if hot-relaoding, stale cache will reflect old locations, if not cleared)
        uniformCache.clear();

        // ZCompile sahders
        const char* vShaderSource = vertexCode.c_str();
        const char* fShaderSource = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512]; // Error msg buffer

        //Vertex
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderSource, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            TraceLog(ErrorType::ERROR, "[Shader Compiler](Vertex): ---VERTEX ERROR STACK: ---\n{}", infoLog);
            glDeleteShader(vertex);
            return;
        }

        // Fragment
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderSource, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            TraceLog(ErrorType::ERROR, "[Shader Compiler](Fragment): --- FRAGMENT ERROR STACK: ---\n{}", infoLog);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return;
        }

        // Create and Link program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            TraceLog(ErrorType::ERROR, "[Shader Compiler](Program): --- PROGRAM ERROR STACK: ---\n{}", infoLog);
            glDeleteProgram(ID); // Forces the Shader obj to become invalid
            ID = 0;
            return;
        }

        // Detach sahders (not strictly required since we delete them afterwards)
        glDetachShader(ID, vertex);
        glDetachShader(ID, fragment);

        // Delete shader components now that they're linked!
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        TraceLog(ErrorType::DEBUG, "[Shader]: Successfully compiled shader program");
    }

    void Shader::bind() {
        if (ID == 0) {
            TraceLog(ErrorType::WARNING, "[Shader]: Attempting to bind an invalid shader");
            return;
        }

        glUseProgram(ID);
    }

    namespace Shaders {
        // Put all pre-defined shaders and factory functions here

        namespace PREBUILT_2D {
            const std::string SHADER_2D_VERTEX = R"(
                #version 330 core
                layout (location = 0) in vec3 aPos;
                layout (location = 1) in vec2 aUV;

                out vec2 TexCoord;

                uniform mat4 model;
                uniform mat4 projection;

                uniform vec4 uvRect; // x=left, y=top, z=right, w=bottom

                void main() {
                    gl_Position = projection * model * vec4(aPos, 1.0);

                    // Check if uvRect is set to a valid range
                    if (uvRect.z > uvRect.x && uvRect.w > uvRect.y){
                        // UV slice defined
                        TexCoord = vec2(
                            uvRect.x + aUV.x * (uvRect.z - uvRect.x),
                            uvRect.y + aUV.y * (uvRect.w - uvRect.y)
                        );
                    } else {
                        // Fallback: normalize aUV from [-0.5,0.5] -> [0,1]
                        TexCoord = aUV + vec2(0.5);
                    }
                }

            )";

            const std::string SHADER_2D_FRAGMENT = R"(#version 330 core
                in vec2 TexCoord;

                out vec4 FragColor;

                uniform sampler2D texture1;
                uniform vec4 uColor;

                void main(){
	                vec4 fragCol = texture(texture1, TexCoord);
	                FragColor = fragCol * uColor;
                }
            )";

        }

        Shader generateDefault2DShader() {
            return Shader(PREBUILT_2D::SHADER_2D_VERTEX, PREBUILT_2D::SHADER_2D_FRAGMENT, false);
        }

    }

}