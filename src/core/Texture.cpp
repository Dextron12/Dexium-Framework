#include <core/Texture.hpp>

#include <string>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char* vertexTextureShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aUV;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main(){\n"
"	gl_Position = projection * model * vec4(aPos, 1.0);\n"
"	TexCoord = aUV;\n"
"}\0";

const char* fragmentTextureShader = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"
"void main(){\n"
"	FragColor = texture(texture1, TexCoord);\n"
"}\0";


Texture2D::Texture2D() : width(0), height(0), format(0) {

	// Generate Mesh object;
	//mesh = std::make_unique<Mesh>();
	//mesh->type = MeshType::USER_DEFINED;

	//Generate texture buffer, Mesh::uplaod generates VBO, VAO & EBO
	glGenTextures(1, &ID);
}

void Texture2D::load(const std::string& filePath, FilterMode mode) {

	//Validate if file exists:
	if (!VFS::exists(filePath)) {
		TraceLog(LOG_ERROR, "[Texture]: Failed to load resource: '%s'", filePath.c_str());
		return;
	}

	if (ID == 0) {
		TraceLog(LOG_ERROR, "GL failed to generate a TextureID");
		return;
	}

	// Bind texture object
	glBindTexture(GL_TEXTURE_2D, ID);

	// Texture filters:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (mode == FilterMode::Linear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Load texture:
	int nrChannels = 0;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		// Dtect format being used:
		if (nrChannels == 1) format = GL_RED;
		if (nrChannels == 2) format = GL_RG;
		if (nrChannels == 3) format = GL_RGB;
		if (nrChannels == 4) format = GL_RGBA;

		// Upload and Generate Mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		TraceLog(LOG_ERROR, "[Textrue]: Failed to read texture: '%s'", filePath.c_str());
		return;
	}
	stbi_image_free(data);

	//Unbind TEXTURE_2D from the texture (Prevents crossing or overflow)
	glBindTexture(GL_TEXTURE_2D, 0);

	TraceLog(LOG_INFO, "[Texture]: Loaded texture: '%s'", std::filesystem::path(filePath).stem().string().c_str());

}

/*
// Assumes that the shader has already been bound by this point
void Texture2D::load(std::string filePath, FilterMode mode) {
	if (isLoaded) {
		TraceLog(LOG_WARNING, "Texture is already loaded. Set relaod flag to force a reload");
		return;
	}

	if (!VFS::exists(filePath)) {
		TraceLog(LOG_ERROR, "[Texture]: Failed to load texture from: '%s'", filePath.c_str());
		return;
	}

	if (ID == 0) {
		TraceLog(LOG_ERROR, "GL failed to generate a textureID");
		return;
	}

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, ID);

	// Texture filters:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (mode == FilterMode::Linear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Load texture:
	int nrChannels = 0;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		// Dtect format being used:
		if (nrChannels == 1) format = GL_RED;
		if (nrChannels == 2) format = GL_RG;
		if (nrChannels == 3) format = GL_RGB;
		if (nrChannels == 4) format = GL_RGBA;

		// Upload and Generate Mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		TraceLog(LOG_ERROR, "[Textrue]: Failed to read texture: '%s'", filePath.c_str());
		return;
	}
	stbi_image_free(data);

	//Unbind TEXTURE_2D from the texture (Prevents crossing or overflow)
	glBindTexture(GL_TEXTURE_2D, 0);

	// Setup & Configure Mesh

	float vertexData[] = {
		//  X      Y     Z      U     V
		 0.5f,  0.5f,  0.0f,  1.0f, 0.0f,   // Top Right
		 0.5f, -0.5f,  0.0f,  1.0f, 1.0f,   // Bottom Right
		-0.5f, -0.5f,  0.0f,  0.0f, 1.0f,   // Bottom Left
		-0.5f,  0.5f,  0.0f,  0.0f, 0.0f    // Top Left
	};

	int indices[] = {
		0, 1, 3,	// First triangle
		1, 2, 3		// Second triangle
	};

	if (mesh == nullptr) {
		TraceLog(LOG_ERROR, "[Texture]: Cannot generate mesh. None was provided");
		return;
	}

	mesh->vertexCount = 4;
	mesh->indexCount = 6;

	/*

	// Sets the position attrib and uploads data. The UV's are uploaded seperatly so i can easily change this code later for sub-texturing
	mesh->upload(vertexData, sizeof(vertexData), []() {
		//Position atrib
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		}, indices, sizeof(indices));
	//mesh->shaderID = "__TexShader";

	
}
*/


/*
void Texture2D::render(glm::vec2 pos, int textureUnit) {
	render(glm::vec4(pos.x, pos.y, 0.0f, 0.0f), textureUnit);
}

void Texture2D::render(glm::vec4 pos, int textureUnit) {
	if (mesh == nullptr) {
		TraceLog(LOG_ERROR, "[Texture]: The itnernal mesh is invalid.");
		TraceLog(LOG_INFO, "[Hint]: Did you try calling Texture2D::load()?");
		return;
	}

	if (mesh->shaderID.empty()) {
		TraceLog(LOG_WARNING, "[Texture]: No Shader is configured for this texture.\nDepending on your hardware, no texture may be rendered");
		return;
	}
	auto shader = AssetManager::getInstance().use<Shader>(mesh->shaderID);
	if (!shader) {
		TraceLog(LOG_ERROR, "[Texture Renderer]: Failed to get shader '%s'", mesh->shaderID.c_str());
		return;
	}

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, ID);
	shader->setInt("texture1", textureUnit);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	if (pos.z == 0 && pos.w == 0) {
		model = glm::scale(model, glm::vec3((float)width, (float)height, 0.0f));
	}
	else {
		model = glm::scale(model, glm::vec3(pos.z, pos.w, 1.0f));
	}
	shader->setMat4("model", model);


	// render the mesh, and pray the the GL gods that the VAO stored the texture data
	mesh->render();
}

*/