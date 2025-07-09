
template<typename T>
bool AssetManager::queryAsset(const std::string& assetID) {
	auto it = assets.find(assetID);
	if (it == assets.end()) {
		TraceLog(LOG_INFO, "[Asset Manager]: The asset: '%s' coult not be found", assetID.c_str());
		return false;
	}

	if (it->second.type == deduceAssetType<T>()) {
		return true;
	}
	else {
		TraceLog(LOG_ERROR, "The asset: '%s' exists but theres a type mismatch or another error has occured", assetID.c_str());
		return false;
	}

	return false;
}


template <typename T>
std::shared_ptr<T> AssetManager::load(const std::string& id, const std::string& filePath) {
	/*
		THIS FUNCTION HAS TWO ENTRY POINTS!!!:
		- Calling it directly (to both register and immediately load the asset.
		- Internally called on a registered asset, when end-user calls use() for the first tiem on that object.
	
		Therefore, this function does it's best to accomidate both. But other functions could loose some 
		defensive coding to ensure its not being too strict on an asset that doesnt meet the criteria (e.g having multiple files) 
	*/

	AssetEntry* entry = nullptr;

	//Check if the asset is already ergistered:
	const auto& it = assets.find(id);
	if (it == assets.end()) {
		// Asset not registered:

		//Check if the file exists:
		if (!VFS::exists(path)) {
			TraceLog(LOG_ERROR, "[Asset Loader]: The asset path '%s' is invalid\n", id.c_str());
			return nullptr;
		}

		// Insert default constructed entry:
		entry = &assets[id];
		entry->type = deduceAssetType<T>();
		entry->registryPath = filePath;
		entry->validatePath = true; // This asset requires a filePath
	}
	else {
		// map entry to register:
		entry = &it->second;
	}

	// Check if internal asset ahs been generated, if so ensure overwrite is allowed
	if (entry->ptr != nullptr) {
		// For now, cannot verify overwrite protections.
		TraceLog(LOG_ERROR, "Cannot overwrite asset: '%s'. Overwrite is prohibited", id.c_str());
		return nullptr;
	}

	// Individual cases for loading the asset:
	switch (entry->type) {
	case AssetType::Texture:
		entry->ptr = std::make_shared<Texture2D>(entry->registryPath);
		break;
	case AssetType::Shader_Type:
		//entry->ptr = std::make_shared<Shader>()
		TraceLog(LOG_INFO, "A call to load a shader was made. But that isnt impl yet");
		break;
	case AssetType::Unkown_Asset: break;
	default: break;
	}
}

/*template<typename T>
std::shared_ptr<T> AssetManager::load(const std::string& id, const std::string& path) {
	// Check if filee xists:
	if (!VFS::exists(path) && (deduceAssetType<T>() != AssetType::Shader_Type)) {
		TraceLog(LOG_ERROR, "[Asset Loader]: The asset path '%s' is invalid", path.c_str());
		return nullptr;
	}

	AssetEntry entry;
	entry.type = deduceAssetType<T>();

	//Check for an unkown type
	if (entry.type == Unkown_Asset) {
		TraceLog(LOG_ERROR, "[Asset Loader]: Cannot load and unkown asset type for: '%s'", id.c_str());		return nullptr;
		return nullptr;
	}

	// Asset cahce validated, laod it.

	//Individual Loaders:
	switch (entry.type) {
	case AssetType::Texture:
		entry.ptr = std::make_shared<Texture2D>(Texture2D(path));
		entry.registryPath = path;
		break;
	case AssetType::Unkown_Asset:
		// Assume we have already caught this error
		break;
	default: break;
	}

	TraceLog(LOG_INFO, "[Asset Loader]: Successfully loaded: '%s'", id.c_str());

	assets.insert_or_assign(id, entry);
	return std::static_pointer_cast<T>(entry.ptr);
}*/

template<typename T>
std::shared_ptr<T> AssetManager::use(const std::string& id) {
	// Check if asset is registered:
	auto entry = assets.find(id);
	if (entry == assets.end()) {
		//No entry exists
		TraceLog(LOG_ERROR, "[Asset Manager]: THe requested asset: '%s' does not exist", id.c_str());
		TraceLog(LOG_INFO, "Did you forget to register the asset using AssetManager::registerAsset()?");
		return nullptr;
	}

	// Check for type mismatches:
	if (entry->second.type != deduceAssetType<T>()) {
		TraceLog(LOG_ERROR, "[Asset Lookup]: Invalid type used for asset: '%s'", id.c_str());
		return nullptr;
	}


	//Chekc if internal asset is loaded
	if (entry->second.ptr == nullptr) {
		if (entry->second.registryPath.empty() && entry->second.validatePath) {
			TraceLog(LOG_ERROR, "[Asset Manager]: The asset: '%s' has an empty path", id.c_str());
			TraceLog(LOG_ERROR, "Ensure your path is correct when you register it");
			return nullptr;
		}
		else {
			// Asset is configured for lazy-loading
			load<T>(entry->first, entry->second.registryPath);
			TraceLog(LOG_INFO, "Load called for asset: '%s'", id.c_str());
		}
	}

	return std::static_pointer_cast<T>(entry->second.ptr);
}

template<typename T, typename... Args>
void AssetManager::registerAsset(const std::string& id, Args&&... args) {
	registerAssetImpl(T{}, id, std::forward<Args>(args)...);
}

/*template<typename T>
void AssetManager::registerAsset(const std::string& assetID, const std::string& assetPath, bool allowOverwrite) {
	//Chekc if path exists:
	auto& res = VFS::resolve(path);
	if (!res) {
		TraceLog(LOG_ERROR, "[Asset Manager]: The path '5s' for assetID: '%s' is invalid", res->c_str(), assetID.c_str());
		return;
	}

	AssetEntry entry;
	entry.registryPath = res.get();
	entry.type = deduceAssetType<T>();
	entry.ptr = nullptr; // Lazy-load when requested.

	// Check if an asset of the same name exists:
	auto it = assets.find(assetID);
	if (it != assets.end()) {
		if (allowOverwrite) {
			TraceLog(LOG_INFO, "[Asset Manager]: Overwriting: '%s' with asset: '%s'", assetID.c_str(), entry.registryPath.c_str());
			assets[assetID] = entry;
		}
		else {
			TraceLog(LOG_ERROR, "[Asset Manager]: Cannot voerwrite '%s'", assetID.c_str());
			return;
		}
	}
	else {
		// Asset doesn't exist, create a new register:
		assets.insert({ assetID, entry });
	}
}*/

template<>
constexpr AssetType AssetManager::deduceAssetType<Texture2D>() {
	return AssetType::Texture;
}

template<>
constexpr AssetType AssetManager::deduceAssetType<Shader>() {
	return AssetType::Shader_Type;
}