
#include "Sprite.hpp"
#include "Texture.hpp"
#include "AssetManager.hpp"

#include <filesystem>

Sprite::Sprite(const std::string& spritePath, std::shared_ptr<Camera> camera) {
	m_camera = camera;

	auto& assets = AssetManager::getInstance();


	// validate path/sprite
	std::unique_ptr<std::string> path = VFS::resolve(spritePath);
	std::string texName;
	if (!path) {
		// Check if its a registered asset:
		if (!assets.queryAsset(spritePath)) {
			TraceLog(LOG_ERROR, "[Sprite]: Texture '%s' couldn't be resolved as it's not registered or the path couldn't be resolved", spritePath.c_str());
			return;
		}
		else {
			// Texture is registered, set its assetID
			texName = spritePath;
		}
	}
	else {
		// Path has been resolved. Register texturre and name
		std::filesystem::path m_path(*path.get());
		// extract name
		if (!m_path.has_stem()) {
			TraceLog(LOG_ERROR, "[Sprite]: Failed to extract file name from the provided path: '%s'", path->c_str());
			return;
		}
		texName = m_path.stem().string(); // Stem() gets the filename without the extension
		
		// Refer to Issue #03
		assets.registerAsset(texName, AssetType::Texture, { *path.get()});
	}

	// Calling asset.use will internally load texture (refer to Issue: #02)
	// Issue #02 expressly opened for this use-case

	// Asset Manager integration into subsystems ahs amde simple things hell. I hope the end-user appreciates the AssetManager

	// Create Material & assign default Shader
	switch (m_camera->type) {
	case ORTHOGRAPHIC:
		// Use default 2D shader
		m_renderable.material = std::make_shared<Material>("__2D-Shader");
		break;
	case PERSPECTIVE:
		// Use default 3D shader
		//m_renderable.material = std::make_shared<Material>("__3D-Shader");
		TraceLog(LOG_WARNING, "[Sprite]: Currently no 3D default shader is provided.\nProvide your own material with preconfigured 3D shader to sprite instead!");
		break;
	case UNDEFINED:
		TraceLog(LOG_INFO, "[Sprite]: Camera does not specify a 2D/3D projection. No default shader can be generated for a user-defined camera\nPlease provide a preconfigured material(With a configured Shader) to Sprite instead");
		break;
	default:
		TraceLog(LOG_WARNING, "[Sprite]: Could not generate a default material.\nPlease manually provide a preconfigured one");

	}
	// Set Mesh texture
	m_renderable.material->setTexture(Material::TextureType::Albedo, texName);
	// Configure Mesh for quad generation
	m_renderable.mesh = MeshFactory::QuadWithUV();

	// Configure Material to use loaded texture (Internally calls AssetManager::use() on the texture register)
	if (m_renderable.material != nullptr) {
		m_renderable.material->setTexture(Material::TextureType::Albedo, "albedo", texName);
	}

	// Configure model scale:
	// Get texture data
	const auto tex = assets.use<Texture2D>(texName);
	m_renderable.transform->scale = glm::vec3(static_cast<float>(tex->width), static_cast<float>(tex->height), 0.0f);

}

Sprite::Sprite(std::shared_ptr<Material> material, const std::string& spritePath, std::shared_ptr<Camera> camera) {
	// Set camera:
	m_camera = camera;

	auto& assets = AssetManager::getInstance();

	// validate path/sprite
	std::unique_ptr<std::string> path = VFS::resolve(spritePath);
	std::string texName;
	if (!path) {
		// Check if its a registered asset:
		if (!assets.queryAsset(spritePath)) {
			TraceLog(LOG_ERROR, "[Sprite]: Texture '%s' couldn't be resolved as it's not registered or the path couldn't be resolved", spritePath.c_str());
			return;
		}
		else {
			// Texture is registered, set its assetID
			texName = spritePath;
		}
	}
	else {
		// Path has been resolved. Register texturre and name
		std::filesystem::path m_path(*path.get());
		// extract name
		if (!m_path.has_stem()) {
			TraceLog(LOG_ERROR, "[Sprite]: Failed to extract file name from the provided path: '%s'", path->c_str());
			return;
		}
		texName = m_path.stem().string(); // Stem() gets the filename without the extension

		// Refer to Issue #03
		assets.registerAsset(texName, AssetType::Texture, { *path.get() });
	}

	// Calling asset.use will internally load texture (refer to Issue: #02)
	// Issue #02 expressly opened for this use-case

	// Asset Manager integration into subsystems ahs amde simple things hell. I hope the end-user appreciates the AssetManager

	// Check if provided material has an attached shader
	if (!material->hasShader()) {
		TraceLog(LOG_ERROR, "[Sprite]: The provided material does not have a valid shader");
		return;
	}
	else {
		m_renderable.material = material;
	}

	// Configure Mesh for quad generation
	m_renderable.mesh = MeshFactory::QuadWithUV();

	// Configure Material to use loaded texture (Internally calls AssetManager::use() on the texture register)
	if (m_renderable.material != nullptr) {
		m_renderable.material->setTexture(Material::TextureType::Albedo, "albedo", texName);
	}

	// Configure model scale:
	// Get texture data
	const auto tex = assets.use<Texture2D>(texName);
	m_renderable.transform->scale = glm::vec3(static_cast<float>(tex->width), static_cast<float>(tex->height), 0.0f);



}

const std::shared_ptr<Material>& Sprite::getMaterial() {
	return m_renderable.material;
}

void Sprite::render(const Transform& transform){
	m_renderable.transform = std::make_unique<Transform>(transform);

	//Bind material (Should eventually be done in a renderer pipeline)
	m_renderable.material->bind();
	m_renderable.material->setUniform("model", m_renderable.transform->toModelMatrix());

	// Rebind texture (per render, in case of using same material)
	//m_renderable

	//Render it
	m_renderable.render(m_camera->getProjection());


}

void Sprite::render(const glm::vec4& pos) {
	m_renderable.transform->position = glm::vec3(pos.x, pos.y, 0.0f);

	m_renderable.transform->scale = glm::vec3(pos.z, pos.w, 0.0f);

	render(*m_renderable.transform.get());
}

void Sprite::update() {

}






// Spritesheet implementation

Spritesheet::Spritesheet(const std::string &fileName) {
	auto& assets = AssetManager::getInstance();

	// Generate a Renderable
	m_sprite = std::make_unique<Renderable>();

	// Create a default 2D Material
	m_sprite->material = std::make_shared<Material>("__2D-Shader");

	// Check if 'fileName' is a path or an asset
	if (!VFS::exists(fileName)) {
		if (assets.queryAsset(fileName)) {
			// filePath is a pre-registered asset. Assign it!
			m_sprite->material->setTexture(Material::TextureType::Albedo, "albedo", fileName); // Stores the pre-registered asset for rendering on the Albedo surface
			m_spriteID = fileName;
		} else {
			auto resFile = VFS::resolve(fileName);
			if (!resFile) {
				// No pre-registered asset or a existing path provided. Abort!
				TraceLog(LOG_ERROR, "[Spritesheet]: Failed to resolve file name '%s'", fileName.c_str());
				return;
			}
			// File path validated. Configure it for loading
			const auto l_texturePath = std::filesystem::path(*resFile.get()).stem();
			assets.registerAsset(l_texturePath.string(), AssetType::Texture, {l_texturePath.string()});
			m_spriteID = l_texturePath.string();
		}
	} else {
		std::cerr << "Hello, World!" << std::endl;
		// File exists, load it! (If a file was found, assume that a path was provided. Assetmanager doesnt usually store ID's as paths)
		const auto l_texturePath = std::filesystem::path(fileName).filename();
		assets.registerAsset(l_texturePath.stem().string(), AssetType::Texture, {l_texturePath.string()});
		m_spriteID = l_texturePath.string();
	}

	// Generate Quad mesh
	m_sprite->mesh = MeshFactory::QuadWithUV();

	// Configure the Material to use the texture2D
	m_sprite->material->setTexture(Material::TextureType::Albedo, "albedo", m_spriteID);

	m_sprite->material->clearUniforms();
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

	//shader->setMat4("projection", camera->getProjection());
	//shader->setMat4("model", transform.toModelMatrix());
	m_sprite->material->setUniform("uvRect", uv);

	//m_mesh->render();
	m_sprite->render(camera->getProjection());



}












