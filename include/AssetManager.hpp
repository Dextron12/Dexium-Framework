#ifndef ASSET_MAN_HPP
#define ASSET_MAN_HPP

#include <unordered_map>
#include <memory>
#include <algorithm>
#include <string>

enum AssetType {
	Texture,
	Spritesheet,
	Anim_Sprite,
	Shader_Type,
	Unkown_Asset
};

struct AssetEntry {
	std::shared_ptr<void> ptr;
	AssetType type = Unkown_Asset;

	//Path vars;
	std::string registryPath; // Should eventually change to a vec to store multiple paths
	bool validatePath;

};

#include <Texture.hpp>
#include <Shader.hpp>

class AssetManager {
public:
	static AssetManager& get();

	static void destroyAsset(const std::string& assetID);

	// Outputs all loaded assets in readable format (command-line output)
	static void listLoadedAssets();

	template<typename T, typename... Args>
	static void registerAsset(const std::string& assetID, Args&&... args);
	void registerAssetsFromManifest(const std::string& manifestPath);

	template<typename T>
	static std::shared_ptr<T> load(const std::string& id, const std::string& path);

	template<typename T> 
	std::shared_ptr<T> use(const std::string& assetID);

	template<typename T>
	static bool queryAsset(const std::string& assetID);
	AssetEntry* getAssetEntry(const std::string& assetID);

	// Individual registers:
	static void registerAssetImpl(Texture2D, const std::string& ID, const std::string& filePath, bool overwrite=false);
	static void registerAssetImpl(Shader, const std::string& assetID, const std::string& vertexPath, const std::string& fragmentPath, bool isFiles = true, bool overwrite = false);

private:
	static std::unordered_map<std::string, AssetEntry> assets;

	AssetManager() = default;

	template<typename T>
	static constexpr AssetType deduceAssetType();


	

};

#include "AssetManager.inl"

#endif