#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>

#include <string>

//std::string SHADER_2D_VERTEX;
//std::string SHADER_2D_FRAGMENT;

namespace Dexium {


	extern const std::string SHADER_2D_VERTEX;
	extern const std::string SHADER_2D_FRAGMENT;

	void displayMat4(const glm::mat4& m);

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

		void setVec2(const std::string& name, glm::vec2 value) const;
		void setVec3(const std::string& name, glm::vec3 value) const;
		void setVec4(const std::string& name, glm::vec4 value) const;

		void setMat4(const std::string& name, glm::mat4 value) const;

	private:
		std::string vertexCode, fragmentCode;
		std::string vertexPath, fragmentPath;
		bool usingFiles;
	};


}


#endif
