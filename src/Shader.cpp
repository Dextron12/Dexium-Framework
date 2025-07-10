
#include <Shader.hpp>

Shader::Shader(const std::string& vertex, const std::string& fragment, bool areFiles) {
	usingFiles = areFiles;

	if (!usingFiles) {
		vertexCode = vertex; fragmentCode = fragment;
	}
	else {
		//Set paths:
		vertexPath = vertex;
		fragmentPath = fragment;

		std::ifstream vShaderFile, fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			//Close files
			vShaderFile.close();
			fShaderFile.close();
			//Convert to strings
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "[Shader Compiler]: File not successfullly read" << std::endl;
		}
	}

}

void Shader::compile(){

	std::printf("[Shader]: Compiling Shader Program\n");

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//Compile Shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "[Shader Compiler: Vertex]: Failed to compile Vertex Shader\n" << infoLog << std::endl;
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "[Shader Compiler: Fragment]: Failed to compile Fragment shader\n" << infoLog << std::endl;
	}

	// Create and Link program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "[Shader Compiler: Program]: Failed to compile Shader Program\n" << infoLog << std::endl;
	}

	// Clean up
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	if (this == nullptr) {
		std::printf("[Error] [Shader Program]: No GL ID registered!\n");
	}
	std::printf("Using shader program: %d\n", ID);
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}