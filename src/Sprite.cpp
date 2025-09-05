
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