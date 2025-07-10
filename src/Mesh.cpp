
#include "window.hpp" // For TraceLog()
#include "Shader.hpp" // For Shader;
#include "AssetManager.hpp" // For AssetManager;

#include <Mesh.hpp>

void Mesh::destroy() {
	if (VBO) glDeleteBuffers(1, &VBO);
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (EBO) glDeleteBuffers(1, &EBO);

	// Prevents memory leaks on gl objects. They dont really need ot be incased in an if statement
	// as GL checks their state regardless if 0 or laoded.
}

void Mesh::upload(const void* vertexData, GLsizeiptr vertexSize, std::function<void()> setupAttributes, const void* indexData, GLsizeiptr indexSize) {
	
	// Validate Vertex data:
	if (!vertexData) {
		TraceLog(LOG_ERROR, "[Mesh Uploader]: No Vertex data provided for Mesh: '%s'", name.c_str());
		TraceLog(LOG_INFO, "[Mesh Uplaoder]: Ensure your Mesh factory Fucntion is working!");
		return;
	}
	if (vertexCount == 0) {
		TraceLog(LOG_ERROR, "[Mesh Uploader]: vertexCount is 0 - nothing can be drawn!");
		return; // No point uplaoding data if nothing will be drawn fromit!
	}

	destroy(); // Clean up any old buffer memory from OpenGL (to rpevent a memory leak!)
	
	TraceLog(LOG_INFO, "Uploading mesh data to GPU");

	// Upload data to GPU
	glGenBuffers(1, &VBO);

	// Bind VAO:
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Check if an EBO is needed
	if (indexData && indexSize) {
		glGenBuffers(1, &EBO);
	}

	// Upload vertex data (VBO)
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds it
	glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, usageHint);

	// Upload element buffer (EBO) (if it exists) NOTE: VBO & VAO in-function, EBO is generated per Meshfactory output
	if (usingEBO()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Binds EBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, usageHint);
	}

	// Configure attributes:
	if (setupAttributes) {
		setupAttributes(); // User-defined attribute layout
	}
	else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	//Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Re-binds the VAO to 0(NO VAO)
	glBindVertexArray(0);
}

void Mesh::render() {
	auto& assets =  AssetManager::getInstance();

	// Get shader info:
	if (!shaderID.empty()) {
		const auto& shader = assets.use<Shader>(shaderID);
		if (shader == nullptr) {
			TraceLog(LOG_INFO, "[Mesh Renderer]: ShaderID: '%s', rendering default shader", shaderID.c_str());
		}
		else {
			shader->use();
		}
	}

	glBindVertexArray(VAO);
	if (usingEBO()) {
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
}

void MeshFactory::defaultLayout3f() {
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}


// Mesh factory funcs:

Mesh MeshFactory::Triangle() {
	Mesh mesh;

	mesh.type = MeshType::TRIANGLE;

	mesh.vertexCount = 3;

	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f,  -0.5f, 0.0f,
	0.0,   0.5f,  0.0f
	};

	mesh.upload(vertices, sizeof(vertices), MeshFactory::defaultLayout3f);

	/*mesh.upload(vertices, sizeof(vertices), []() {
		// Positon at location 0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL-FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	});*/ // <-- example of adding custom layouts

	return mesh;
}

Mesh MeshFactory::Rectangle() {
	Mesh mesh;

	mesh.type = MeshType::RECTANGLE;

	// Generate EBO;
	mesh.indexCount = 6;
	mesh.vertexCount = 4;


	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	mesh.upload(vertices, sizeof(vertices), MeshFactory::defaultLayout3f, 
	indices, sizeof(indices));

	return mesh;
}