
#include <AssetManager.hpp>

std::unordered_map<std::string, AssetEntry> AssetManager::assets;

AssetManager& AssetManager::get() {
	static AssetManager instance;
	return instance;
}

void AssetManager::destroyAsset(const std::string& assetID) {
	auto it = assets.find(assetID);
	if (it != assets.end()) {
		//Asset exits remove it.
		TraceLog(LOG_INFO, "[AssetManager}: Unloading asset: '%s'", assetID.c_str());

		// Check for copies of the asset ptr
		if (it->second.ptr.use_count() > 1) {
			TraceLog(LOG_WARNING, "[Asset Manager}: Asset '%s' still has %d references!", assetID.c_str());
		}

		assets.erase(it);
		return;
	}

	TraceLog(LOG_INFO, "[Asset Manager}: No asset found for ID: '%s' Nothing to unload.", assetID.c_str());
}

AssetEntry* AssetManager::getAssetEntry(const std::string& assetID) {
	auto it = assets.find(assetID);
	if (it != assets.end()) {
		return &it->second;
	}
	else {
		TraceLog(LOG_WARNING, "A Request for Asset-cache on '%s' could not be rpovided as one doesn't exist", assetID.c_str());
		return nullptr;
	}

}

void AssetManager::registerAssetsFromManifest(const std::string& manifestPath) {
	
}

void AssetManager::listLoadedAssets() {
	std::cout << "\n\n----==== [Registered Assets] ====----\n";
	int registryCount = 0;
	for (auto [name, data] : assets) {
		bool loaded = true;
		if (data.ptr == nullptr) loaded = false;

		std::string reg = loaded ? "Loaded" : "Registered";

		std::printf("[Entry %d]: ------------------------------------------------------------------------\n", registryCount);
		std::printf("Asset Name: '%s', Status: '%s', Registered Path: '%s'\n\n\n", name.c_str(), reg.c_str(), data.registryPath.c_str());
		registryCount++;
	}
	std::cout << "----==== [Registered Assets] ====----\n";
}

void AssetManager::registerAssetImpl(Texture2D, const std::string& ID, const std::string& filePath, bool overwrite) {
	AssetEntry entry;

	auto res = VFS::resolve(filePath);
	if (!res) {
		TraceLog(LOG_ERROR, "[Texture Register]: The path '%s' for asset '%s' is invalid", filePath.c_str(), ID.c_str());
		return;
	}

	// Path should also be validated:
	entry.validatePath = true;

	//Check if the same asset exists:
	const auto& it = assets.find(ID);
	if (it != assets.end() && !overwrite) {
		TraceLog(LOG_ERROR, "[Texture Register]: The asset: '%s' cannot be overwritten. It is locked", ID.c_str());
		return;
	}

	entry.registryPath = res->c_str();
	entry.type = AssetType::Texture;
	//entry.ptr = std::make_shared<Texture2D>(res->c_str());
	entry.ptr = nullptr;

	assets.insert({ ID, entry });

}


void AssetManager::registerAssetImpl(Shader, const std::string& assetID, const std::string& vertexPath, const std::string& fragmentPath, bool isFiles, bool overwrite) {
	// Specialized func to capture both files of the Shader prpgram. Also to overload it to parse direct strings for the shaders.
	
	// Check if a Shader Asset of the same anme already exists:
	auto it = assets.find(assetID);
	if (it != assets.end() && !overwrite) {
		TraceLog(LOG_ERROR, "[Asset Manager: (Shader Specalization)]: Cannot overwrite existing shader program: '%s", it->first);
		return;
	}

	// Check fi the data are files or data
	if (isFiles) {
		// check vertexPath:
		if (!VFS::resolve(vertexPath)) {
			TraceLog(LOG_ERROR, "[Asset Manager]: vertexPath: '%s' for Shader: '%s' is invalid", vertexPath.c_str(), assetID.c_str());
			return;
		}

		if (!VFS::resolve(fragmentPath)) {
			TraceLog(LOG_ERROR, "[Asset Manager]: fragmentPath: '%s' for Shader: '%s' is invalid", fragmentPath.c_str(), assetID.c_str());
			return;
		}
	}

	AssetEntry entry;
	entry.type = AssetType::Shader_Type;
	//entry.ptr = std::make_shared<Shader>();
	entry.ptr = nullptr;

	// Create register:
	assets.insert({ assetID, entry });
}