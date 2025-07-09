#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>

#include <stb_image.h>

#include <window.hpp>

struct Texture2D {
	int width, height;
	int format;

	unsigned int ID;

	Texture2D() : width(0), height(0), format(0), ID(0) {}
	Texture2D(const std::string& texturePath);

	bool isValid();
};

#endif