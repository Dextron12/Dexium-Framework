#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <string>
#include <functional>

enum class MeshType {
	TRIANGLE,
	RECTANGLE,
	CIRCLE,
	LINE,
	USER_DEFINED
};

class Mesh {
public:
	unsigned int VBO = 0; // (Vertex Buffer Object) A buffer that holds info on positions, etc
	unsigned int VAO = 0; // (Vertex Attribute State Tracker) essentially caches any buffer bound to it. So this can be bound to re-bind the VBO and EBO if being used
	unsigned int EBO = 0; // (Element Buffer Object) Allows user to set indices and reduce vertex counts
	
	int vertexCount = 0; int indexCount = 0;

	GLenum drawMode = GL_TRIANGLES;
	GLenum usageHint = GL_STATIC_DRAW; // Hint to GL how you will be using the Mesh data

	std::string shaderID;	// Will swap for a Material when materials are implemented
	std::string name;		// Used internally for debuggin, using the assetID here will suffice
	MeshType type = MeshType::USER_DEFINED;

	Mesh() = default;
	~Mesh(){ destroy(); }

	void upload(const void* vertexData, GLsizeiptr vertexSize, std::function<void()> setupAttributes = nullptr,  const void* indexData = nullptr, GLsizeiptr indexSize = 0); // Uplaods Mesh data to the GPU
	void render(); // Renders the Mesh

private:

	// Helper functions
	bool usingEBO() { return EBO != 0; };
	void destroy();
};

class MeshFactory {
public:
	// Layout Helpers:
	static void defaultLayout3f(); // Position (location = 0)

	// Factory Functions:
	static Mesh Triangle();
	static Mesh Rectangle();
};

#endif