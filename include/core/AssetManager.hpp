//
// Created by Dextron12 on 23/11/2025.
//

#ifndef DEXIUM_ASSETMANAGER_HPP
#define DEXIUM_ASSETMANAGER_HPP

#include <filesystem>
#include <unordered_map>
#include <memory>

#include <utils/ID.hpp>
#include <core/Error.hpp>

#include "Dexium.hpp"

// Forward declares for assets
namespace Dexium {
    class Texture2D;
}


namespace Dexium::Core {



    struct FileDef {
        std::string data;
        std::filesystem::file_time_type fileTime; // Storage for last modified file time

        bool isPath = false; // If false, registryPath si raw data, if true registryPath is a relative path
        bool isHotReloadable = false; // Determiens if its configured for hot realod or not
    };

    struct AssetEntry {
        std::vector<FileDef> registryPaths;
        std::unique_ptr<void, std::function<void(void*)>> ptr = nullptr; // The function allows a custom deleter to be stored for the void* ptr
        std::type_index type = typeid(void); // Stores the asset type being used

        bool isLoaded = false;

        //Optional type specific config
        using ConfigVariant = std::variant<
            std::monostate,
            Config::TextureConfig
        >;

        ConfigVariant config;
    };

    template<typename T>
    void* constructAsset(const AssetEntry& entry);

    // USES SFINAE rules to test for various patterns of a load function
    //It does:
    // Try: #1 Does T.load(Args...) exist?
    // Try: #2: If not, return false
    // Where:
    // #1 is a accepts all params and #2 is default load()
    // We then sue constexpr to check if the rpovidded Args matches the internal laod fucniton
    template<typename T, typename... Args>
    struct has_load_with_args {
    private:
        template<typename U>
        static auto test(int) -> decltype(std::declval<U&>().load(std::declval<Args>()...), std::true_type{});

        template<typename>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    class AssetManager {
    public:
        AssetManager() = default;

        template <typename T, typename ConfigT = std::monostate>
        void registerAsset(Utils::ID assetID, const std::vector<std::string>& filePaths, ConfigT config = {}, bool configHotReload = false) {
            // First, check if the asset already is registered
            auto it = m_assets.find(assetID);
            if (it != m_assets.end()) {
                // Asset witht he same ID already exists!, warna nd early exit
                TraceLog(LOG_WARNING, "[AssetMan]: THe asset '{}' is already registered", assetID.str());
                return;
            }

            // Otherwise, cerate a new asset
            AssetEntry entry;
            for (const auto& d : filePaths) {
                // Feed in each file path/raw data into the entry
                entry.registryPaths.emplace_back(FileDef{d});
                FileDef& def = entry.registryPaths.back(); // Get last assignment in the vector

                // Validate the rel path and if enabled, configure file tracking(for hot-reload)
                if (auto absPath = VFS::resolve(def.data)) {
                    // a valid abs path can be generated from the provided relative
                    def.isPath = true;
                    // Configure hot-reloading dependant on state
                    if (configHotReload) {
                        // Enable hot-relaoding for this file
                        //filesystem::last_write_time can fail!
                        try {
                            def.fileTime = std::filesystem::last_write_time(*absPath);
                            def.isHotReloadable = true;
                        } catch (const std::filesystem::filesystem_error& err) {
                            TraceLog(LOG_ERROR, "[AssetMan]: Failed to retrieve last_write_time for '{}'.Reason: {}", assetID.str(), err.what());
                        }
                    }
                }
                // If path cannot be validated, it likely is raw data
                // But this is why internal asset systems also self report on their given paths/data
                // We obviously can't hot-reload baked-in goods or raw data provided by the end-user

            }
            // The ConfigT allows the end-user to provide a strongly typed configuration for the asset in question
            // each config should be defined in Dexium::Config and be a struct of useful vars for that asset
            // the loader then can do what it needs to in order to parse that config along the line and ensure the asset is created/managed properly

            entry.config = config; // Stores the end-user config profile
            entry.ptr = nullptr; // Ensures the newly registered asset is only registered until first use, where the loader will generate this ptr
            entry.type = typeid(T); // stores the asset type being registered


            m_assets[assetID] = std::move(entry); // Ensure there is no copy made (AssetEntry can be rather large with config and files)
        }

        bool queryAsset(const Utils::ID& assetID);

        void unload(const Utils::ID& assetID);
        // Not happy with oyur configured Assetmanager, this is the Nuke button!!
        void clear();

        template<typename T, typename... Args>
        T* use(Utils::ID assetID, Args&&... args) {
            // Find the asset
            auto it = m_assets.find(assetID);
            if (it == m_assets.end()) {
                // No asset found, warn and return
                TraceLog(LOG_WARNING, "[AssetMan]: An unregistered asset '{}' called use()!", assetID.str());
                return nullptr;
            }

            AssetEntry& entry = it->second;

            // Check for first load
            if (!entry.isLoaded) {
                // Generate the asset
                //entry.ptr = std::unique_ptr<void>(new T(std::forward<Args>(args)...));
                entry.ptr.reset(constructAsset<T>(entry),
                    Deleter{ [](void* p){delete static_cast<T*>(p); }});

                // Check for load function on asset
                if constexpr (has_load_with_args<T, std::string, Config::FilterMode>::value) {
                    if (auto* cfg = std::get_if<Config::TextureConfig>(&entry.config)) {
                        static_cast<T*>(entry.ptr.get())->load(
                            entry.registryPaths[0].data, cfg->filterMode);
                    }
                }
                entry.isLoaded = true;
            }

            // Return typed ptr
            return static_cast<T*>(entry.ptr.get());
        }

    private:
        std::unordered_map<Utils::ID, AssetEntry> m_assets;

        struct Deleter {
            void (*fn)(void*) = nullptr;
            void operator()(void* p) const {fn (p); }
        };

    };


}


#endif //DEXIUM_ASSETMANAGER_HPP