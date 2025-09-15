

// Forward declarations for types
class Texture2D;
class Shader;
class Mesh;
class Sprite;
class Spritesheet;

template<typename T>
struct AssetTraits;

template <>
struct AssetTraits<Texture2D> {
	static constexpr AssetType type = AssetType::Texture;
};

template <>
struct AssetTraits<Shader> {
	static constexpr AssetType type = AssetType::Shader;
};

template <>
struct AssetTraits<Mesh> {
	static constexpr AssetType type = AssetType::Mesh;
};

/*
template <>
struct AssetTraits<Sprite> {
	static constexpr AssetType type = AssetType::Sprite;
};
 --- NEEDS IMPLEMENTING ---
 */

template <typename T>
void AssetManager::registerAsset(const std::string& id, T&& asset, std::function<bool(const T&)> validator) {
	constexpr AssetType type = AssetTraits<T>::type;

	if (validator && !validator(asset)) {
		TraceLog(LOG_ERROR, "[Asset-Registrar]: Validation for '%s' failed. Check output logs", id.c_str());
		return;
	}

	// Check for pre-exisiting registered asset
	auto it = assets.find(id);
	if (it != assets.end()) {
		TraceLog(LOG_ERROR, "[Asset-Registrar]: Already registered '%s'", id.c_str());
		return;
	}

	AssetEntry entry;
	entry.type = type;
	entry.ptr = std::make_shared<T>(std::move(asset));
	assets[id] = entry; // Create new entry
}

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
			TraceLog(LOG_ERROR, "[Asset-Man]: Failed to load asset: '%s'", id.c_str());
			return nullptr;
		}
	}

	// Downcast to the correct type
	return std::static_pointer_cast<T>(entry.ptr);
}