//
// Created by Dextron12 on 18/12/25.
//

#ifndef DEXIUM_TEXTURE_HPP
#define DEXIUM_TEXTURE_HPP

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/VFS.hpp>
#include <core/Error.h>

#include "fmt/printf.h"

namespace Dexium::Core {

    // Bitwise enum class for flag operator representation of  possible Texture configurations
    enum class TexFlags : uint32_t {
        None = 0,

        // Filtering
        Nearest = 1 << 0,
        Linear = 1 << 1,

        // Wrapping
        Repeat = 1 << 2,
        ClampEdge = 1 << 3,


        // Mipmaps
        Mipmaps = 1 << 4
    };

    // Bitwise operators for the enum class
    constexpr TexFlags operator|(TexFlags a, TexFlags b) {
        return static_cast<TexFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr TexFlags operator&(TexFlags a, TexFlags b) {
        return static_cast<TexFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b) );
    }

    constexpr TexFlags operator^(TexFlags a, TexFlags b) {
        return static_cast<TexFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b) );
    }

    constexpr TexFlags operator~(TexFlags a) {
        return static_cast<TexFlags>(~static_cast<uint32_t>(a) );
    }

    inline TexFlags& operator|=(TexFlags& a, TexFlags b) {
        a = a | b;
        return a;
    }

    inline TexFlags& operator&=(TexFlags& a, TexFlags b) {
        a = a & b;
        return a;
    }

    // Helper function to check if a flags exists
    constexpr bool hasFlags(TexFlags value, TexFlags flag) {
        return (value & flag) != TexFlags::None;
    }

    class Texture {
    public:
        GLuint texID = 0;
        TexFlags flags = TexFlags::None;

        int width = 0, height = 0, nrChannels = 0;

        ~Texture() {
            // Prevents a failed texture from deleting an invalid texture(SEGFAULT)
            if (texID != 0){
                //glDeleteTextures(1, &texID); // CUrrentlyc auses SEGFAULT!! Neds to be fixed!!
                texID = 0; // Done for engine edbugging. If engine sees it, warns and ignores this texture
            }
        }

        // Prevent copying (As this allows a double delete of the texture)
        Texture() = default;
        Texture(Texture&& other) noexcept;
        Texture& operator=(const Texture&) = delete;

        // Checks stored flags and uploads params according to the flag specifications
        // REQUIRES: The texture bound to GL_TEXTURE_2D
        void uploadParameters() {
            //Filtering
            if (hasFlags(flags, TexFlags::Linear)) {
                GLint minFilter = hasFlags(flags, TexFlags::Mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            } else if (hasFlags(flags, TexFlags::Nearest)) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }


            // Wrapping
            if (hasFlags(flags, TexFlags::Repeat)) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            } else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            // mIPMAPS
            if (hasFlags(flags, TexFlags::Mipmaps)) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }

        void bind(int textureUnit) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texID);
        }

        bool load(const std::filesystem::path& path) {
            auto p = VFS::resolve(path);
            if (p.empty()) {
                // Couldnt get an ABS path extraction. failed to laod
                TraceLog(ErrorType::ERROR, "[Texture]: Failed to load texture, path '{}' is invalid", p.string());
                return false;
            }

            // Load texture
            // Flip so (0,0) is bottom-left for OpenGL UVs
            stbi_set_flip_vertically_on_load(true);

            unsigned char* data = stbi_load(p.c_str(), &width, &height, &nrChannels, 0);
            if (!data) {
                TraceLog(ErrorType::ERROR, "[Texture]: Failed to load texture from '{}'", p.c_str());
                return false;
            }

            // Create new TexID, generate and bind
            glGenTextures(1, &texID);
            glBindTexture(GL_TEXTURE_2D, texID);

            // Get internal format of texture
            GLenum format = GL_RGB;
            if (nrChannels == 4) format = GL_RGBA;
            else if (nrChannels == 1) format = GL_RED;

            // Uplaod data to GPU
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            // Free CPU side mem. Img data is now stored in VRAM,a ccessed by ID.
            // Can store as many textures as supported by VRAM cap, but exceeding cap results in GPU paging
            // The TraceLogger can eb extened tod etect for such eroneous GL state errors and output a warniong when OUT_OF_MEM

            uploadParameters();

            stbi_image_free(data);

            return true;

        }


    };

}

#endif //DEXIUM_TEXTURE_HPP