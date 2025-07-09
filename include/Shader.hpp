#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
	unsigned int ID;

	// Just stores the data. call compile() for shader propgram to be built.
	Shader() = default;
	Shader(const std::string& vertex, const std::string& fragment, bool areFiles = true);

	void compile();


	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:
	std::string vertexCode, fragmentCode;
	std::string vertexPath, fragmentPath;
	bool usingFiles;
};

#endif
