
// Material.hpp - Dexium

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <variant>
#include <unordered_map>
#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "AssetManager.hpp"
//#include "Texture.hpp"

namespace Dexium {
	class Material {
	public:
		using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;


		enum class TextureType {
			Albedo,
			Normal,
			Roughness,
			Shininess,
			User_Defined

		};

		Material(const std::string& shaderID);


		// Sets a specified uniform for the configured shader
		void setUniform(const std::string& name, const UniformValue& value);
		void clearUniforms();

		// Binds a texture to the material (Uses pre-registered asset)
		void setTexture(const TextureType type, const std::string& name, const std::string& textureID = "");
		// Binds a texture to the material (
		// Apply material (and its properties) before drawing
		void bind() const;

		// Returns if a shader si attached or not.
		bool hasShader();

	private:
		std::string m_shaderID;

		//Uniforms & Textures
		std::unordered_map<std::string, UniformValue> m_uniforms;
		std::unordered_map<std::string, std::string> m_textures;
	};


	// Shoudle evntually be moved into its own header and a proper renderer made out of it
	struct Renderable {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		std::unique_ptr<Transform> transform;

		Renderable() : transform(std::make_unique<Transform>()) {}

		// Deep copy support
		Renderable(const Renderable& other) : mesh(other.mesh), material(other.material), transform(std::make_unique<Transform>(*other.transform)) {}

		Renderable& operator=(const Renderable& other) {
			if (this != &other) {
				mesh = other.mesh;
				material = other.material;
				transform = std::make_unique<Transform>(*other.transform);
			}
			return *this;
		}

		// Accept Move
		Renderable(Renderable&&) noexcept = default;
		Renderable& operator=(Renderable&&) noexcept = default;

		void render(const glm::mat4& projection) {
			// Validate state:
			if (!mesh) { TraceLog(LOG_ERROR, "[Renderer]: Invalid state - No bound mesh"); return; }
			if (!material) { TraceLog(LOG_ERROR, "[Renderer]: Invalid state - No bound material"); return; }

			material->bind();
			material->setUniform("projection", projection);
			material->setUniform("model", transform->toModelMatrix());
			mesh->render();


		}

	};


}

#endif