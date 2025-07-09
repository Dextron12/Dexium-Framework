
#include "window.hpp"

#include <AssetManager.hpp>

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f,  -0.5f, 0.0f,
	0.0,   0.5f,  0.0f
};

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main(){\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

#include <glad/glad.h>


int main(int argc, char* argv[]) {

	WindowContext win("Dexium Tests", 1280, 720);

	AssetManager& assets = AssetManager::get();

	assets.registerAsset<Texture2D>("pig", "Tests/Assets/Animals/Pig/Pig_01.png", false);

	assets.registerAsset<Shader>("shader", vertexShaderSource, fragmentShaderSource, false);
	const auto& shader = assets.use<Shader>("shader");

	// Generates unique VBO
	unsigned int VBO, VAO; // VAO --> "Vertex Attribute State Tracker", a lite-weight data structure that "remember"s the VBO layout. Can swab between VAO binds to render different states.
	glGenBuffers(1, &VBO);

	// Generates unique VAO & binds it. Binda  VAO before, binding VBO to GL_ARRAY_BUFFER & modifying it, to eprsist the state.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Think of this as telling GL the data we have.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Think of this as telling GL how to traverse the data we have provided.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	while (win.appState) {
		win.startFrame();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//std::printf("deltaTime: %f,  FPS: %d\n", win.deltaTime, win.FPS);

		glBindVertexArray(VAO); // Binds the VBO state configuration
		glDrawArrays(GL_TRIANGLES, 0, 3);

		win.endFrame(); 
	}
	return 0;
}