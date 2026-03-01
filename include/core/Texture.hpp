//
// Created by Dextron12 on 18/12/25.
//

#ifndef DEXIUM_TEXTURE_HPP
#define DEXIUM_TEXTURE_HPP

#include <glad/gl.h>

#include <core/BitwiseFlag.hpp>

#include <filesystem>

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

    template <>
    struct EnableBitmaskOperators<TexFlags> {
        static constexpr bool value = true;
    };



    class Texture {
    public:
        GLuint texID = 0;
        TexFlags flags = TexFlags::None;

        int width = 0, height = 0, nrChannels = 0;

        ~Texture();

        // Prevent copying (As this allows a double delete of the texture)
        Texture() = default;
        Texture(Texture&& other) = default;
        Texture& operator=(const Texture&) = delete;

        // Checks stored flags and uploads params according to the flag specifications
        // REQUIRES: The texture bound to GL_TEXTURE_2D
        void uploadParameters() const;

        bool load(const std::filesystem::path& path);


    };

}

#endif //DEXIUM_TEXTURE_HPP