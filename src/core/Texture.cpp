//
// Created by ethan on 1/3/26.
//

#include <core/Texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/VFS.hpp>
#include <core/Error.hpp>

Dexium::Core::Texture::~Texture() {
    if (texID != 0) {
        // glDeleteTextures(1, &texID); // Currently causes a segfault
        texID = 0;
    }
}

void Dexium::Core::Texture::uploadParameters() const {
    //Filtering
    if (hasFlag(flags, TexFlags::Linear)) {
        GLint minFilter = hasFlag(flags, TexFlags::Mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (hasFlag(flags, TexFlags::Nearest)) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // Wrapping
    if (hasFlag(flags, TexFlags::Repeat)) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // mIPMAPS
    if (hasFlag(flags, TexFlags::Mipmaps)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

bool Dexium::Core::Texture::load(const std::filesystem::path& path) {
    //Resolve path
    auto p = VFS::resolve(path);
    if (p.empty()) {
        // Failed to find path
        TraceLog(LogLevel::ERROR, "[Texture]: Failed to load texture from '{}'", path.string());
        return false;
    }

    // Load Texture
    // Flip so (0,0) is bottom-left for OpenGL UVs
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(p.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        TraceLog(LogLevel::ERROR, "[Texture]: Failed to load texture from: '{}'", p.string());
        return false;
    }

    //Generate & bind new texID
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    //Get internal format of texture
    GLenum format = GL_RGB;
    if (nrChannels == 4) format = GL_RGBA;
    else if (nrChannels == 1) format = GL_RED;

    // set texture filters
    uploadParameters();

    // Upload data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    //Free loaded stb data
    stbi_image_free(data);
    return true;
}