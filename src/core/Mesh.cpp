
#include "window.hpp" // For TraceLog()
#include "Shader.hpp" // For Shader;
#include "AssetManager.hpp" // For AssetManager;

#include <Mesh.hpp>

void Mesh::destroy() {
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);
	if (VAO) glDeleteVertexArrays(1, &VAO);

	VBO = 0;
	VAO = 0;
	EBO = 0;

	// Prevents memory leaks on gl objects. They dont really need ot be incased in an if statement
	// as GL checks their state regardless if 0 or laoded.
}

void Mesh::upload(const void* vertexData, GLsizeiptr vertexSize, std::function<void()> setupAttributes, const void* indexData, GLsizeiptr indexSize) {
	
	// Validate Vertex data:
	if (!vertexData) {
		//TraceLog(LOG_ERROR, "[Mesh Uploader]: No Vertex data provided for Mesh: '%s'", name.c_str());
		TraceLog(LOG_INFO, "[Mesh Uplaoder]: Ensure your Mesh factory Fucntion is working!");
		return;
	}
	if (vertexCount == 0) {
		TraceLog(LOG_ERROR, "[Mesh Uploader]: vertexCount is 0 - nothing can be drawn!");
		return; // No point uplaoding data if nothing will be drawn fromit!
	}

	destroy(); // Clean up any old buffer memory from OpenGL (to rpevent a memory leak!)

	// Bind VAO:
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Upload vertex data (VBO)
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds it
	glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, usageHint);


	//Check if an EBO is needed
	if (indexData && indexSize) {
		glGenBuffers(1, &EBO);
	}
	else if (EBO != 0) {
		TraceLog(LOG_WARNING, "[Mesh Uploader]: No index data has been provided but a EBO is in use!");
	}

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
		TraceLog(LOG_INFO, "Setting attrib pointer 0 to default");
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	//Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Re-binds the VAO to 0(NO VAO)
	glBindVertexArray(0);
}

void Mesh::upload(const std::vector<float>& vertices, std::function<void()> setupAttributes, const std::vector<unsigned int>& indices) {
	upload(vertices.data(), vertices.size() * sizeof(float), setupAttributes, indices.data(), indices.size() * sizeof(unsigned int));
}

void Mesh::render() {

	if (VAO == 0 || EBO == 0) {
		TraceLog(LOG_ERROR, "[Mesh]: Invalid buffers detected!\nVAO: %d, VBO: %d, EBO: %d", VAO, VBO, EBO);
	}

	glBindVertexArray(VAO);
	if (usingEBO()) {
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

	//Unbind VAO (Prevents fun little bugs i spend days crying over!) -> Seriously do NOT touch!
	glBindVertexArray(0);
}

void MeshFactory::defaultLayout3f() {
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind currently bound array (Prevents overlap, when building/editing multipe VBO's)
	glBindVertexArray(0);
}

void MeshFactory::defaultTexture() {
	//Pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// UVs
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
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

Mesh MeshFactory::unitRectangle() {
	Mesh mesh;
	mesh.type = MeshType::RECTANGLE;
	mesh.indexCount = 6; // Generates EBO
	mesh.vertexCount = 4;

	std::vector<float> vertices = {
		// Pos					// Uvs
		0.0f, 0.0f, 0.0f,		0,0,	// Top left
		1.0f, 0.0f, 0.0f,		1,0,	// Bottom left
		1.0f, 1.0f, 0.0f,		1,1,	// Bottom right
		0.0f, 1.0f, 0.0f,		0,1		// Top right
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0
	};

	mesh.upload(vertices, MeshFactory::defaultTexture, indices);

	return mesh;
}

std::shared_ptr<Mesh> MeshFactory::QuadWithUV(const std::vector<glm::vec2>* UVs) {
	auto mesh = std::make_shared<Mesh>();
	mesh->type = MeshType::RECTANGLE;

	// Generate EBO
	mesh->indexCount = 6;
	mesh->vertexCount = 4;

	std::vector<glm::vec3> positions = {
		glm::vec3(0.5f, 0.5f, 0.0f),	// top right
		glm::vec3(0.5f, -0.5f, 0.0f),	// bottom right
		glm::vec3(- 0.5f, -0.5f, 0.0f),	// bottom left
		glm::vec3(- 0.5f, 0.5f, 0.0f)	// top left
	};

	// Check for UV data, if not, generate it
	std::vector<glm::vec2> uvData;
	if (UVs) {
		uvData = *UVs;
	}
	else {
		// generate default:
		uvData = {
			glm::vec2(1.0f, 1.0f),		// top right
			glm::vec2(1.0f, 0.0f),		// bottom right
			glm::vec2(0.0f, 0.0f),		// bottom left
			glm::vec2(0.0f, 1.0f)		// top left
		};
	}

	// Indice data:
	std::vector<unsigned int> indices = {
		0, 1, 3,	// First triangle
		1, 2, 3		// Second triangle
	};

	// Convert and join data
	std::vector<float> m_vertices;
	size_t vertexCount = positions.size();
	m_vertices.clear();
	m_vertices.reserve(vertexCount * 5); // 3 pos + 2 UV

	for (size_t i = 0; i < vertexCount; ++i) {
		// Position
		m_vertices.push_back(positions[i].x);
		m_vertices.push_back(positions[i].y);
		m_vertices.push_back(positions[i].z);

		// UV
		m_vertices.push_back(uvData[i].x);
		m_vertices.push_back(uvData[i].y);
	}

	mesh->upload(m_vertices, MeshFactory::defaultTexture, indices);

	return mesh;

}