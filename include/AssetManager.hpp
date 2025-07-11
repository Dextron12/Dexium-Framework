#ifndef ASSET_MAN_HPP
#define ASSET_MAN_HPP

#include <unordered_map>
#include <memory>
#include <algorithm>
#include <vector>
#include <functional>
#include <string>

#include <Mesh.hpp>

enum class AssetType {
	Texture,
	Spritesheet,
	Anim_Sprite,
	Shader,
	Mesh,
	Unkown_Asset
};

struct AssetEntry {
	AssetType type = AssetType::Unkown_Asset;
	std::vector<std::string> registryPaths;
	std::shared_ptr<void> ptr = nullptr;

	bool fromFiles = true; // Load a file OR use raw data provided in the filePaths
	bool validatePath = true; // When true, Uses VFS::resolve & VFS::exists to validate paths.
};

class AssetManager {
public:
	// SIngleton accessor
	static AssetManager& getInstance() {
		static AssetManager instance;
		return instance;
	}

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	void registerAsset(const std::string& id, AssetType type, const std::vector<std::string>& paths, bool fromFile = true, bool validate = true);
	void registerAsset(const std::string& id, MeshType meshType, std::shared_ptr<Mesh> meshData = nullptr);

	template<typename T>
	std::shared_ptr<T> use(const std::string& id);

	template <typename T>
	void registerLoader(AssetType type, std::function<std::shared_ptr<T>(const AssetEntry&)> loader);

	void unload(const std::string& id);
	void clear();

private:
	AssetManager() = default;
	~AssetManager() = default;

	std::unordered_map<std::string, AssetEntry> assets;
	std::unordered_map < AssetType, std::function<std::shared_ptr<void>(const AssetEntry&)>> loaders;
};

#include <Texture.hpp>
#include <Shader.hpp>

#include <AssetManager.inl>



#endif