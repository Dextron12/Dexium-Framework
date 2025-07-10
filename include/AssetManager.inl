



template<typename T>
void AssetManager::registerLoader(AssetType type, std::function<std::shared_ptr<T>(const AssetEntry&)> loader) {
	// Wraps the laoder into a type erased version and stores it
	loaders[type] = [loader](const AssetEntry& entry)->std::shared_ptr<void> {
		return loader(entry); // Calls user-supplied loader
	};
}

template<typename T>
std::shared_ptr<T> AssetManager::use(const std::string& id) {
	auto it = assets.find(id);
	if (it == assets.end()) {
		TraceLog(LOG_ERROR, "[Asset-Man]: Asset not found: '%s'", id.c_str());
		return nullptr;
	}

	std::cout << it->first << std::endl;

	AssetEntry& entry = it->second;

	// Lazy-load if needed
	if (!entry.ptr) {
		auto loaderIt = loaders.find(entry.type);
		if (loaderIt == loaders.end()) {
			TraceLog(LOG_ERROR, "[Asset-Man]: No loader for asset type: %d", static_cast<int>(entry.type));
			return nullptr;
		}

		// Calls a type-erased laoder lambda
		entry.ptr = loaderIt->second(entry);

		if (!entry.ptr) {
			TraceLog(LOG_ERROR, "[Asset-Man]: Failed ot laod asset: '%s'", id.c_str());
			return nullptr;
		}
	}

	// Downcast to the correct type
	return std::static_pointer_cast<T>(entry.ptr);
}