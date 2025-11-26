//
// Created by Dextron12 on 23/11/2025.
//

#include <core/AssetManager.hpp>

#include <core/VFS.hpp>

bool Dexium::Core::AssetManager::queryAsset(const Utils::ID& assetID) {
    auto it = m_assets.find(assetID);
    if (it != m_assets.end()) {
        return true;
    } else { return false; }
}

void Dexium::Core::AssetManager::unload(const Utils::ID& assetID) {
    auto it = m_assets.find(assetID);
    if (it != m_assets.end()) {
        // Asset found, free it
        //it->second.ptr.release(); // Explcitly free the underlying resource. unique_ptr should call the destructor to the object from here
        m_assets.erase(it);
        TraceLog(LOG_INFO, "[AssetMan]: '{}' has been unloaded", assetID.str());
    }
    // No need to do anything if the asset wasn't found!
}

void Dexium::Core::AssetManager::clear() {
    m_assets.clear();
}

// Asset type specialisations

template<>
void *Dexium::Core::constructAsset<Sprite>(const AssetEntry& entry) {
    return new Sprite(entry.registryPaths[0].data);
}


