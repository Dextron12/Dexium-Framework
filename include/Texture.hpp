#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Mesh.hpp>

#include <window.hpp>
#include <AssetManager.hpp>

enum class FilterMode {
	Nearest,
	Linear
};

// Generates a quad mesh and renders a etxture onto that quad
class Texture2D {
public:
	int width, height;
	int format;

	unsigned int ID;

	Texture2D();

	// Use FilterMode::Nearest for pixel art sprites, Linear for a blended natural look
	void load(std::string filePath, FilterMode mode = FilterMode::Nearest);

	void render(glm::vec4 pos, int textureUnit = 0);
	void render(glm::vec2 pos, int textureUnit = 0);

	void setShader(const std::string& shaderID);

private:

	bool isLoaded = false; // If true, Texture::Load() ahs been called

	std::unique_ptr<Mesh> mesh;

};

#endif