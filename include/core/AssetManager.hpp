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

	void registerDefaultLoaders(); // Ensure this is eventually put into an engine init function

	void registerAsset(const std::string& id, AssetType type, const std::vector<std::string>& paths, bool fromFile = true, bool validate = true);
	void registerAsset(const std::string& id, MeshType meshType, std::shared_ptr<Mesh> meshData = nullptr);

	bool queryAsset(const std::string& name);

	template<typename T>
	std::shared_ptr<T> use(const std::string& assetID);

	template<typename T>
	// Use this registerAsset call if you registering a pre-defined asset
	void registerAsset(const std::string& id, T&& asset, std::function<bool(const T&)> validator = nullptr);

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