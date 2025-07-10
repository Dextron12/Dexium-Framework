
#include "window.hpp"
#include "Timer.hpp"

#include <AssetManager.hpp>
#include <Mesh.hpp>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main(){\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main(){\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}";

int main(int argc, char* argv[]) {
	WindowContext win("Dexium Tests", 1280, 720);

	auto& assets = AssetManager::getInstance();

	assets.registerAsset("basicShader", AssetType::Shader_Type, { vertexShaderSource, fragmentShaderSource }, false);
	const auto& shader = assets.use<Shader>("basicShader");
	
	Mesh mesh = MeshFactory::Rectangle();
	mesh.shaderID = "basicShader";

	Timer UIKey_Debounce(300);
	
	while (win.appState){
		win.startFrame();

		if (glfwGetKey(win.window, GLFW_KEY_F1) == GLFW_PRESS && UIKey_Debounce.isFinished()) {
			win.toggleWireFrame();
			UIKey_Debounce.reset();
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		mesh.render();

		win.endFrame();
	}
}