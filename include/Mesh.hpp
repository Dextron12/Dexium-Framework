#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <string>
#include <memory>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class MeshType {
	TRIANGLE,
	RECTANGLE,
	CIRCLE,
	LINE,
	USER_DEFINED
};

struct Transform {
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 rotation{ 0.0f, 0.0f, 0.0f }; // Euler angles in radians (pitch, yaw, roll)

	Transform() = default;
	Transform(const glm::vec3& pos, const glm::vec3& sca = { 1,1,1 }, const glm::vec3& rot = { 0,0,0 })
		: position(pos), scale(sca), rotation(rot) {
	}

	glm::mat4 toModelMatrix() const {
		glm::mat4 model(1.0f);

		// transform checks:
		// refer to Bug: #04

		if (scale.x <= 1 || scale.y <= 1) {
			//TraceLog(LOG_WARNING, "The model scale is likely too small for anything to render! See Bug: #04!\nScale(%f, %f, %f)", scale.x, scale.y, scale.z);
			std::printf("[WARNING]: The model scale is likely too small for anything to render! sEE bUG: #04\nScale(%f, %f, %f)\n", scale.x, scale.y, scale.z);
		}

		model = glm::translate(model, position);

		// Apply rotations in ZYX order (common in graphics engines)
		model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
		model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));

		model = glm::scale(model, scale);
		return model;
	}
};

class Mesh {
public:
	unsigned int VBO = 0; // (Vertex Buffer Object) A buffer that holds info on positions, etc
	unsigned int VAO = 0; // (Vertex Attribute State Tracker) essentially caches any buffer bound to it. So this can be bound to re-bind the VBO and EBO if being used
	unsigned int EBO = 0; // (Element Buffer Object) Allows user to set indices and reduce vertex counts
	
	int vertexCount = 0; int indexCount = 0;
	MeshType type = MeshType::USER_DEFINED;

	GLenum drawMode = GL_TRIANGLES;
	GLenum usageHint = GL_STATIC_DRAW; // Hint to GL how you will be using the Mesh data

	Transform transform;
	//std::shared_ptr<Material> mat;

	Mesh() = default;
	~Mesh(){ destroy(); }

	void upload(const void* vertexData, GLsizeiptr vertexSize, std::function<void()> setupAttributes = nullptr,  const void* indexData = nullptr, GLsizeiptr indexSize = 0); // Uplaods Mesh data to the GPU
	void upload(const std::vector<float>& vertices, std::function<void()> setupAttributes, const std::vector<unsigned int>& indices);
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
	
	static void defaultTexture(); // Positon (location = 0), UVs (location = 1)

	// Factory Functions:
	static Mesh Triangle();
	static Mesh Rectangle();
	static std::shared_ptr<Mesh> QuadWithUV(const std::vector<glm::vec2>* UVs = nullptr);

	// Unit Factory Functions (ranegs from [0,1] so its compatible for top-left(2D) projections
	// All unit factory functions must provide both vertices & unit UVs
	static Mesh unitRectangle();
};

#endif