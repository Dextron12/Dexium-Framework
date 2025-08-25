
#include "Material.hpp"

Material::Material(const std::string& shaderID) {
	if (!AssetManager::getInstance().queryAsset(shaderID)) {
		// Shader program does not exist!
		TraceLog(LOG_ERROR, "[Material]: No Shader program: '%s' exists!\nMaterial will not render", shaderID.c_str());
		m_shaderID.empty();
		return; // early return to prevent material initalisation.
	}
	else {
		m_shaderID = shaderID;
	}
}

void Material::setUniform(const std::string& name, const UniformValue& value) {
	m_uniforms[name] = value;
}

void Material::setTexture(const TextureType, const std::string& name, const std::string& textureID) {
	std::string schName;
	if (textureID == "" || textureID.size() == 0) {
		schName = name;
	}
	else { schName = textureID; }

	// Validate that a Texture asset exists:
	if (!AssetManager::getInstance().queryAsset(schName)) {
		// No texture exists:
		TraceLog(LOG_WARNING, "[Material]: No texture: '%s' could be found", schName);
		return;
	}

	// Set the textureID
	m_textures[name] = schName;
}

void Material::bind() const {
	// Get AssetManager handle:
	auto& assets = AssetManager::getInstance();

	// Fetch shader program:
	std::shared_ptr<Shader> shader;
	if (!m_shaderID.empty()) {
		shader = assets.use<Shader>(m_shaderID);
	}
	else {
		TraceLog(LOG_ERROR, "[Material]: Failed to retieve & bind shader program: '%s'", m_shaderID.c_str());
		return;
	}

	shader->use();

	// Upload uniforms
	for (const auto& [name, value] : m_uniforms) {
		// Sometimes I do things thinking its smart, until its time to implement that... Which brings me to the below monstrosoty
		std::visit([&](auto&& v) {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, int>) {
				shader->setInt(name, v);
			}
			else if constexpr (std::is_same_v<T, bool>) {
				shader->setBool(name, v);
			}
			else if constexpr (std::is_same_v<T, float>) {
				shader->setFloat(name, v);
			}
			else if constexpr (std::is_same_v<T, glm::vec2>) {
				shader->setVec2(name, v);
			}
			else if constexpr (std::is_same_v<T, glm::vec3>) {
				shader->setVec3(name, v);
			}
			else if constexpr (std::is_same_v<T, glm::vec4>) {
				shader->setVec4(name, v);
			}
			else if constexpr (std::is_same_v<T, glm::mat4>) {
				shader->setMat4(name, v);
			}
			else {
				TraceLog(LOG_WARNING, "[Material]: The uniform: '%s' attempts to set an unsupported value!", name);
			}
			}, value);
	}	// I must reiterate how cool lambda's are! Templates, not so much

	// Bind Textures: Is left up tot he Texture function.
	// However, how should we manage a material using multiple textures onto a 3D shape
	// Or a textureAtlas onto a 3D shape like a cube?
	

	// Textures are handled in the Mesh::render function.
	// Eventually a renderer will encapsulate the Texture@d, Shader & Mesh objects
	// and all renderering will be handled by the renderer.

}