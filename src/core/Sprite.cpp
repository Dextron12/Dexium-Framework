
#include "Sprite.hpp"
#include "Texture.hpp"
#include "AssetManager.hpp"

#include <filesystem>

Sprite::Sprite(const std::string& fileName) {
	// Gte assets
	auto& assets = AssetManager::getInstance();

	// Renderable creation
	m_sprite = std::make_unique<Renderable>();

	// Dewfault 2D amterial
	m_sprite->material = std::make_shared<Material>("__2D-Shader");

	// Generate Quad Mesh (With UV data))
	m_sprite->mesh = MeshFactory::QuadWithUV();

	// Validate || Generate Texture2D (Assetmanager handles registration and loading)

	const auto& l_name = VFS::resolve(fileName);
	if (!l_name) {
		// File does not exist. Check if its pre-registered:
		if (assets.queryAsset(fileName)) {
			m_spriteID = fileName;
		} else {
			// File does not exist and no pre-registered asset
			TraceLog(LOG_WARNING, "[Sprite]: No texture named: '%s' could be located", fileName.c_str());
			return;
		}
	} else {
		// Extract assetID from fileName
		auto l_path = std::filesystem::path(*l_name.get()); // <-- RAW abs path
		std::string name = l_path.filename().stem().string(); // Gets fileName without the extension
		//Register new asset
		assets.registerAsset(name, AssetType::Texture, {l_path.string()});
		m_spriteID = name;
	}

	// COnfigure Material to use Texture
	m_sprite->material->setTexture(Material::TextureType::Albedo, "albedo", m_spriteID);

	// Clear uniforms to prevent weird bugs
	m_sprite->material->clearUniforms();

}

Sprite::~Sprite(){}

void Sprite::setShader(const std::string& shaderID) {
	if (AssetManager::getInstance().queryAsset(shaderID)) {
		m_shaderID = shaderID;
	} else {
		TraceLog(LOG_WARNING, "[Spritesheet]: THe provided shader '%s' does not exist", shaderID.c_str());
	}
}

void Sprite::drawSprite(const glm::vec4 pos, const glm::vec3 rot) {
	auto& assets = AssetManager::getInstance();

	auto shader = assets.use<Shader>(m_shaderID);
	if (!shader) {
		TraceLog(LOG_ERROR, "[Sprite]: Failed to fetch shader program: '%s'", m_shaderID.c_str());
		return;
	}
	shader->use(); // If same shader, GL ignores this state change

	// Get Texture2D
	const auto& l_texture = assets.use<Texture2D>(m_spriteID);
	if (!l_texture) {
		TraceLog(LOG_ERROR, "[Sprite]: Could not fetch Texture2D: '%s'.", m_spriteID.c_str());
		return;
	}
	auto& l_transform = m_sprite->transform;
	l_transform->position = glm::vec3(pos.x, pos.y, 0.0f);
	//l_transform->scale = glm::vec3(l_texture->width, l_texture->height, 1.0f);
	l_transform->rotation = rot;
	// Check if a custom scale has been set
	if (pos.z != 0 && pos.w != 0) {
		l_transform->scale = glm::vec3(pos.z, pos.w, 1.0f);
	} else {
		// Use the texture size as the render size
		l_transform->scale = glm::vec3(l_texture->width, l_texture->height, 1.0f);
	}

	// Configure Shader
	//shader->setMat4("projection", camera->getProjection());
	//shader->setMat4("model", l_transform->toModelMatrix());
	m_sprite->material->setUniform("uvRect", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	m_sprite->material->setUniform("uColor", glm::vec4(255,255,255,255)/255.0f);

	//m_sprite->material->bind(); // Renderable::render also calls this function!

	m_sprite->render(camera->getProjection());

	// Unbind Current texture from unit
	glBindTexture(GL_TEXTURE_2D, 0); // Prevents further quircky bugs over the same texture unit!
}






// Spritesheet implementation

Spritesheet::Spritesheet(const std::string &fileName) {
	auto& assets = AssetManager::getInstance();

	// Generate a Renderable
	m_sprite = std::make_unique<Renderable>();

	// Create a default 2D Material
	m_sprite->material = std::make_shared<Material>("__2D-Shader");

	const auto& l_name = VFS::resolve(fileName);
	if (!l_name) {
		// Check if provided name is a pre-registered asset
		if (assets.queryAsset(fileName)) {
			// Pre-registered asset of same name exists, use it
			m_spriteID = fileName;
		} else {
			// No existign asset or configured path defined. Cannot proceed!
			TraceLog(LOG_ERROR, "[Spritesheet]: Failed to register texture: '%s'. It cannot be located", fileName.c_str());
			return;
		}
	} else {
		const auto l_texturePath = std::filesystem::path(*l_name.get());
		std::string assetName = l_texturePath.filename().stem().string();
		// Register the new asset
		assets.registerAsset(assetName, AssetType::Texture, {l_texturePath.string()});
		m_spriteID = assetName;
	}

	// Generate Quad mesh
	m_sprite->mesh = MeshFactory::QuadWithUV();

	// Configure the Material to use the texture2D
	m_sprite->material->setTexture(Material::TextureType::Albedo, "albedo", m_spriteID);

	m_sprite->material->clearUniforms(); // Uhmm, Does this clear uniforms or uniforms + textures?
}

Spritesheet::~Spritesheet() {

}

void Spritesheet::setSprite(const std::string &spriteID, const glm::vec4 &spriteDim) {
	auto it = m_frames.find(spriteID);
	if (it != m_frames.end()) {
		TraceLog(LOG_WARNING, "[Spritesheet]: The subSprite '%s' is already defined.", spriteID.c_str());
		return;
	}

	// Maybe do some bounds checks here?

	m_frames[spriteID] = spriteDim;
}

void Spritesheet::remSprite(const std::string &spriteID) {
	if (m_frames.erase(spriteID) == 0) {
		TraceLog(LOG_WARNING, "[Spritesheet]: Redundant removal call of subSprite '%s'.", spriteID.c_str());
	}
}

void Spritesheet::setShader(const std::string &shaderID) {
	if (AssetManager::getInstance().queryAsset(shaderID)) {
		m_shaderID = shaderID;
	} else {
		TraceLog(LOG_WARNING, "[Spritesheet]: THe provided shader '%s' does not exist", shaderID.c_str());
	}
}

void Spritesheet::load(const std::string &fileName) {
	/*if (!fileName.empty()) {
		// replace fileName (Allowed for hot-reload support)
		m_texturePath = fileName;
	}

	if (m_texturePath.empty()) {
		TraceLog(LOG_ERROR, "[Spritesheet]: Invalid configuration. No spritesheet to load!");
		return;
	}

	if (fileName != m_texturePath) {
		//use updated path:
		//m_rend
	}
	*/

	/* Current No real need for a laod function */

}

void Spritesheet::drawSprite(const std::string &spriteID, const glm::vec3 pos, glm::vec3 scale, glm::vec3 rot) {
	auto &assets = AssetManager::getInstance();

	auto shader = assets.use<Shader>(m_shaderID);
	shader->use(); // Shouldn't effect perf if its the same shader being used.

	auto it = m_frames.find(spriteID);
	if (it == m_frames.end()) {
		TraceLog(LOG_WARNING, "[Spritesheet]: No Sprite '%s' exists.\nDid you forget to add one?" , spriteID.c_str());
		return;
	}

	const auto& l_texture = assets.use<Texture2D>(m_spriteID);
	if (!l_texture) {
		TraceLog(LOG_ERROR, "[Spritesheet]: Could not fetch texture '%s'", m_spriteID.c_str());
		return;
	}

	float texW = static_cast<float>(l_texture->width); // Whole sprite width
	float texH = static_cast<float>(l_texture->height); // Whole sprite height

	glm::vec4 uv;
	uv.x = it->second.x / texW;		// left
	uv.y = it->second.y / texH;		// right
	uv.z = (it->second.x + it->second.z) / texW;		// width
	uv.w = (it->second.y + it->second.w) / texH;		// height


	auto& l_transform = m_sprite->transform;
	//Transform transform(pos, scale, rot);
	// Reconfigure scale to match the subTexture w & h
	l_transform->position = pos;
	l_transform->scale = glm::vec3(it->second.z, it->second.w, 1.0f); // Force px space coords for subTexture size
	l_transform->rotation = rot;

	shader->setMat4("projection", camera->getProjection());
	shader->setMat4("model", l_transform->toModelMatrix());
	m_sprite->material->setUniform("uvRect", uv);
	m_sprite->material->setUniform("uColor", glm::vec4(255,255,255,255) / 255.0f);

	m_sprite->material->bind();
	//m_mesh->render();
	m_sprite->render(camera->getProjection());

	// Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

const std::string& Spritesheet::getTextureID() {
	return m_spriteID;
}












