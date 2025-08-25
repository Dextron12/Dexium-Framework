
#include <AssetManager.hpp>

// Register Dexium default laoders on static init:
namespace  {
	struct DefaultLoaderRegistrar {
		DefaultLoaderRegistrar() {
			auto& mgr = AssetManager::getInstance();

			// Texture2D Loader:
			/*mgr.registerLoader<Texture2D>(AssetType::Texture, [](const AssetEntry& entry) -> std::shared_ptr<Texture2D> {
				//Check type:
				if (entry.type != AssetType::Texture) {
					TraceLog(LOG_ERROR, "[Asset-Man]: Asset is not a registered Texture2D");
					return nullptr;
				}
				if (entry.registryPaths.empty()) {
					TraceLog(LOG_ERROR, "[Asset-Man]: Cannot create a Texture2D resource without a filePath");
					return nullptr;
				}
				return std::make_shared<Texture2D>(entry.registryPaths[0]);
			});
			*/

			// Texture2D Loader
			mgr.registerLoader<Texture2D>(AssetType::Texture, [](const AssetEntry& entry) -> std::shared_ptr<Texture2D> {
				if (entry.type != AssetType::Texture) {
					TraceLog(LOG_ERROR, "[Asset-Man]: Asset is not a registered Texture2D");
					return nullptr;
				}
				if (entry.registryPaths.empty()) {
					TraceLog(LOG_ERROR, "[Asset-Man]: Cannot create a Texture2D resource without a filePath");
					return nullptr;
				}
				return std::make_shared<Texture2D>();
				});

			// Shader Loader:
			mgr.registerLoader<Shader>(AssetType::Shader, [](const AssetEntry& entry) -> std::shared_ptr<Shader> {
				if (entry.type != AssetType::Shader) {
					TraceLog(LOG_ERROR, "[Asset-Man]: Asset is not a registered Shader");
					return nullptr;
				}
				if (entry.registryPaths.size() < 2) {
					TraceLog(LOG_ERROR, "[Asset-Man]: A shader requiers a vertexPath & fragmentPath\n(OR: provide srcCode fo each as the path args)");
					return nullptr;
				}
				auto ptr = std::make_shared<Shader>(entry.registryPaths[0], entry.registryPaths[1], entry.fromFiles);
				ptr->compile();
				return ptr;
				});

			// Add more Dexium Res loaders here when implemented!
		}
	};
};

void AssetManager::registerDefaultLoaders() {
	static DefaultLoaderRegistrar registrar;
	(void)registrar;
}

void AssetManager::registerAsset(const std::string& id, AssetType type, const std::vector<std::string>& paths, bool fromFile, bool validate) {
	
	// check for validation:
	if (validate && fromFile) {
		for (const std::string& file : paths){
			if (!VFS::resolve(file)) {
				TraceLog(LOG_ERROR, "Failed to resolve path: '%s' for asset: '%s'", file.c_str(), id.c_str());
				return;
			}
		}
	}

	// Check if an entry already exists:
	const auto& it = assets.find(id);
	if (it != assets.end()) {
		TraceLog(LOG_ERROR, "[Asset-Man]: Cannot re-register asset: '%s', it already is registered", id.c_str());
		return;
	}

	// register entry:
	AssetEntry entry;
	entry.fromFiles = fromFile; entry.validatePath = validate;
	entry.type = type;
	entry.registryPaths = paths;
	entry.ptr = nullptr;

	assets.insert_or_assign(id, entry);
}

void AssetManager::registerAsset(const std::string& id, MeshType type, std::shared_ptr<Mesh> meshData) {
	AssetEntry entry;
	entry.fromFiles = false;

	if (meshData != nullptr) {
		// Create a new register:
		entry.ptr = meshData;
	}
	else {
		switch (type) {
		case MeshType::TRIANGLE:
			// uSE THE fACTORY TO GENERATE A MESH
			entry.ptr = std::make_shared<Mesh>(MeshFactory::Triangle());
			break;
		case MeshType::RECTANGLE:
			entry.ptr = std::make_shared<Mesh>(MeshFactory::Rectangle());
			break;
		default: break;
		}

		if (type == MeshType::USER_DEFINED) {
			TraceLog(LOG_ERROR, "[Asset-Man]: Cannot register a user-defined Mesh without the Mesh data");
			return;
		}

	}

	assets.insert_or_assign(id, entry);
}

// Simply checks of the asset is registered. Note: This func makes noa ttempt at valdiating internal asset data!
bool AssetManager::queryAsset(const std::string& assetID) {
	if (assets.find(assetID) != assets.end()) {
		return true;
	}
	else {
		return false;
	}
}

void AssetManager::unload(const std::string& id) {
	//check if asset exists
	const auto& it = assets.find(id);
	if (it != assets.end()) {
		TraceLog(LOG_INFO, "[Asset-Man]: Unloading Resource: '%s'", id.c_str());

		//Check ptr' counts:
		if (it->second.ptr.use_count() > 1) {
			TraceLog(LOG_WARNING, "[Asset-Man]: Resource: '%s' still has '%d' references", id.c_str(), it->second.ptr.use_count());
		}

		assets.erase(it);
	}

	// ELSE: Silently quit, no need to log if there is nothing to delete
}

void AssetManager::clear() {
	if (assets.empty()) return;

	TraceLog(LOG_INFO, "[Asset-Man]: Clearing %d assets", assets.size());

	std::vector<std::string> idsToUnload;
	for (const auto& it : assets) {
		idsToUnload.push_back(it.first);
	}

	for (const auto& id : idsToUnload) {
		unload(id);
	}
}

/*
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


*/