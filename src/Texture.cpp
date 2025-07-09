
#define STB_IMAGE_IMPLEMENTATION
#include <Texture.hpp>

Texture2D::Texture2D(const std::string& filePath) {

	//Load Texture
	int nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		TraceLog(LOG_ERROR, "Failed to load texture: '%s'", filePath.c_str());
		return;
	}
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Set texture wrapping.filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Identify format
	format = GL_RGB;
	if (nrChannels == 1) format = GL_RED;
	if (nrChannels == 3) format = GL_RGB;
	if (nrChannels == 4) format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Free data
	stbi_image_free(data);

}

bool Texture2D::isValid() {
	if (ID <= 0) return false;
	else return true;
}