
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

void Shader::setVec2(const std::string& name, glm::vec2 value) const {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, glm::vec4 value) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& name, glm::mat4 mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void displayMat4(const glm::mat4& m) {
	for (int row = 0; row < 4; ++row) {
		std::cout << "| ";
		for (int col = 0; col < 4; ++col) {
			std::cout << m[col][row] << " ";
		}
		std::cout << " |\n";
	}
}






const std::string SHADER_2D_VERTEX = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

uniform vec4 uvRect; // x=left, y=top, z=right, w=bottom

void main() {
	gl_Position = projection * model * vec4(aPos, 1.0);

	// Check if uvRect is set to a valid range
	if (uvRect.z > uvRect.x && uvRect.w > uvRect.y){
		TexCoord = vec2(
			uvRect.x + aUV.x * (uvRect.z - uvRect.x),
			uvRect.y + aUV.y * (uvRect.w - uvRect.y)
		);
	} else {
		TexCoord = aUV; // fallback to default UVs
	}
}
)";

const std::string SHADER_2D_FRAGMENT = R"(#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main(){
	//FragColor = texture(texture1, TexCoord);
	vec4 texColor = texture(texture1, TexCoord);
	FragColor = mix(vec4(0.3, 0.6, 0.3, 1.0), texColor, texColor.a);
}
)";