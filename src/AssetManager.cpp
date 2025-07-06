
#include "AssetManager.hpp"

std::unordered_map<std::string, AssetEntry> AssetManager::assets;
SDL_Renderer* AssetManager::renderer = nullptr;

AssetManager& AssetManager::get() {
	static AssetManager instance;
	return instance;
}

void AssetManager::init(SDL_Renderer* renderer) {
	AssetManager::get().renderer = renderer;
}

void AssetManager::destroyAsset(const std::string& assetID) {
	//Check if the assetID exists, if so destroy it.
	auto it = assets.find(assetID);
	if (it != assets.end()) {
		//Asset exists, destroy it.
		log(INFO, "[AssetManager}: Unloading asset: '%s'", assetID.c_str());

		// Check for copies of the asset ptr:
		if (it->second.ptr.use_count() > 1) {
			log(WARNING, "[AssetManager}: Asset '%s' still has %d references!", assetID.c_str());
		}
		assets.erase(it);
	}

	log(INFO, "[AssetManager]: No asset found for Id: '%s'. Nothing to unload.", assetID.c_str());
	// Otherwise, quitely do nothing, no need to warn the end-user about ana sset that doesnt exist at this stage.
}

AssetEntry* AssetManager::getAssetEntry(const std::string& assetID) {
	auto it = assets.find(assetID);
	if (it != assets.end()) {
		return &it->second;
	}
	else {
		log(WARNING, "A Request for Asset-cache on '%s' could not be provided as one doesn't exist", assetID.c_str());
		return nullptr;
	}
}

void AssetManager::registerAssetsFromManifest(const std::string& manifestPath, bool allowOverwrite) {
	std::ifstream file(manifestPath);
	if (!file.is_open()) {
		log(Error, "[Asset Manager]: Failed to open manifest: '%s'", manifestPath.c_str());
		return;
	}

	nlohmann::json j;
	try {
		file >> j;
	}
	catch (std::exception& e) {
		log(Error, "[Asset Manager]: JSON parse error in manifest: %s", e.what());
		return;
	}

	// ==== LOAD TEXTURES ====
	if (j.contains("textures")) {
		for (auto& [assetID, path] : j["textures"].items()) {
			auto resolved = VFS::resolve(path);
			if (resolved.success) {
				// reltive texture path resolved into abs path
				// load and create the texture asset
				load<Texture>(assetID, resolved.path.string());
			}
		}
	}

}

void AssetManager::listLoadedAssets() {
	std::cout << std::left << std::setw(20) << "\n\n----==== [ REGISTERED ASSETS ] ====----\n";
	int registryCount = 0;
	for (auto [name, data] : assets) {
		bool loaded = true;
		if (data.ptr == nullptr) loaded = false;

		std::string reg = loaded ? "Loaded" : "Registered";

		std::printf("[Entry %d]: ------------------------------------------------------------------\n", registryCount);
		std::printf("Asset Name: '%s', Status: %s, Path: '%s'\n\n\n", name.c_str(), reg.c_str(), data.registryPath.c_str());
		registryCount++;
	}
	std::cout << std::left << std::setw(20) << "----==== [ REGISTERED ASSETS ] ====----\n";

}