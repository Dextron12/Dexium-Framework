
#include "core/Material.hpp"

Dexium::Material::Material(const std::string& shaderID) {
	if (!AssetManager::getInstance().queryAsset(shaderID)) {
		// Shader program does not exist!
		TraceLog(LOG_ERROR, "[Material]: No Shader program: '{}' exists!\nMaterial will not render", shaderID);
		m_shaderID.clear();
		return; // early return to prevent material initialisation.
	}
	else {
		m_shaderID = shaderID;
	}
}



bool Dexium::Material::hasShader() {
	return AssetManager::getInstance().queryAsset(m_shaderID);
}

void Dexium::Material::setUniform(const std::string& name, const UniformValue& value) {
	m_uniforms[name] = value;
}

void Dexium::Material::clearUniforms() {
	m_uniforms.clear();
}

void Dexium::Material::setTexture(const TextureType type, const std::string& name, const std::string& textureID) {
	std::string schName;
	if (textureID == "" || textureID.size() == 0) {
		schName = name;
	}
	else { schName = textureID; }

	// Validate that a Texture asset exists:
	auto tex = AssetManager::getInstance().use<Texture2D>(schName);
	if (!tex) {
		// No texture exists:
		TraceLog(LOG_WARNING, "[Material]: No texture: '%s' could be found", schName);
		return;
	}


	/*
	 * --- DEXIUM TEXTURE UNITS ---
	 * 1 - Albedo
	 */

	// CConfigure target texture unit
	switch (type) {
		case TextureType::Albedo:
			tex->textureUnit = 1;
			break;
		case TextureType::Normal:
			tex->textureUnit = 2;
			break;
		default:
			TraceLog(LOG_WARNING, "[Material]: TextureType '%d' is not currently supported!", type);
			break;

	}

	// Set the textureID
	m_textures[name] = schName;
}

void Dexium::Material::bind() const {
	// Get AssetManager handle:
	auto& assets = AssetManager::getInstance();

	// Fetch shader program:
	std::shared_ptr<Shader> shader;
	if (!m_shaderID.empty()) {
		shader = assets.use<Shader>(m_shaderID);
	}
	else {
		TraceLog(LOG_ERROR, "[Material]: Failed to retrieve & bind shader program: '%s'", m_shaderID.c_str());
		return;
	}

	shader->use();

	// Upload uniforms
	for (const auto& [name, value] : m_uniforms) {
		// Sometimes I do things thinking it's smart, until it's time to implement that... Which brings me to the below monstrosity
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
	}	// I must reiterate how cool lambdas are! Templates, not so much

	// Bind Textures: Is left up to the Texture function.
	// However, how should we manage a material using multiple textures onto a 3D shape
	// Or a textureAtlas onto a 3D shape like a cube?
	

	// Textures are handled in the Mesh::render function.
	// Eventually a renderer will encapsulate the Texture@d, Shader & Mesh objects
	// and all rendering will be handled by the renderer.

	int textureUnit = 0;
	for (auto& [name, tex] : m_textures) {
		// First validate texture (An unlikely step,an s no invalid textures should exist)
		if (!assets.queryAsset(tex)) {
			TraceLog(LOG_ERROR, "Material configured with an invalid texture: '%s'", name.c_str());
			return;
		}
		const std::shared_ptr<Texture2D>& texture = assets.use<Texture2D>(tex);
		if (!texture) {
			TraceLog(LOG_ERROR, "Material field to fetch configured texture: '%s'", name.c_str());
			return;
		}

		int unit;
		if (texture->textureUnit >= 0) {
			// TextureUnit explicitly assigned, respect it
			unit = texture->textureUnit;
		}
		else {
			// Auto-assign from the running counter
			unit = textureUnit;
			texture->textureUnit = unit; // Persist the assignment
			textureUnit++;
		}

		// Resolve GLSL sampler name:
		std::string glName = texture->samplerName.empty()
			? "texture" + std::to_string(unit) : texture->samplerName;

		//Bind & use texture
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, texture->ID);

		//std::cout << "Texture Unit: " << glName << std::endl;

		shader->setInt(glName, unit);

		//TraceLog(LOG_INFO, "[%s] is using samplerID: %i", tex.c_str(), unit);

	}
}