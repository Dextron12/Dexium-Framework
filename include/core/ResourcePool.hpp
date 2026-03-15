//
// Created by ethan on 14/3/26.
//

#ifndef DEXIUM_RESOURCEPOOL_HPP
#define DEXIUM_RESOURCEPOOL_HPP

#include <memory>
#include <unordered_map>
#include <typeindex> // To cast between T and type without dynamic_cast(RTTI casting)

// I still don't like this UUID/handle system, somethingbetter needs to be designed for ResourcePools
#include <utility>
#include <vector>
#include <utils/ID.hpp>

// A ResourcePool is a type-specific unordered_map that stores cached entries of that type
// Type-specification avoids dynamic_cast, however, means I need to dance with my nemesis... templates

namespace Dexium::Private::Interfaces {
    // Base Interface for ResourcePool generation
    struct IPool {
        virtual ~IPool() = default;
    };
}

namespace Dexium::Core {

    //ResourceHanlde provides meta -data over a resource and is a Generational Handle system allowing for O(1) lookups in the resource pools
    // A quick to copy handle/ID for the underlying resource
    template <typename T>
    class ResourceHandle{
    public:
        //Generational handle vars:
        uint32_t index = 0;
        uint32_t generation = 0;
        Utils::UUID id;

        std::string label;

        ResourceHandle() = default;
        ResourceHandle(uint32_t index, uint32_t generation, Utils::UUID id, std::string label = ""):
        index(index), generation(generation), id(id), label(std::move(label)) {}

        [[nodiscard]] bool valid() const {
            return generation != 0;
        }
    };

    template<typename T>
    class ResourceSlot {
    public:
        uint32_t generation = 1;
        std::unique_ptr<T> resource;
        Utils::UUID uuid;
    };


}



// Interfaces anmespace stores engine interfaces
namespace Dexium::Private::Interfaces {

    template <typename T>
    class ResourcePool : public IPool {
    public:
        Core::ResourceHandle<T> add(std::unique_ptr<T> resource) {
            uint32_t index;

            if (!freeSlots.empty()) {
                // Use next free slot instead of creating a new one
                index = freeSlots.back();
                freeSlots.pop_back();
            } else {
                //Create a new slot
                index = slots.size();
                slots.emplace_back();
            }

            //Fetch slot
            auto& slot = slots[index];
            //Generate UUID
            auto uuid = Utils::UUID::Generate();

            slot.resource = std::move(resource);
            slot.uuid = uuid;

            //Store conversion factor
            uuidToIndex[uuid] = index;

            //Create & return ResHandle
            //return {index, slot.generation, uuid}; //Implicit construction doesnt seem to work well with Clangd compiler
            return Core::ResourceHandle<T>(index, slot.generation, uuid);
        }

        //Fetch a ResourceHandle safely(Perform generation check)
        T* get(const Core::ResourceHandle<T>& handle) {
            // If handles references an old generation, it fails safely
            if (handle.index >= slots.size()) {
                return nullptr;
            }

            auto& slot = slots[handle.index];

            if (slot.generation != handle.generation) {
                return nullptr;
            }

            return slot.resource.get();
        }

        void remove(const Core::ResourceHandle<T>& handle) {
            if (handle.index >= slots.size()) {
                return;
            }

            auto& slot = slots[handle.index];

            if (slot.generation != handle.generation) {
                return;
            }

            uuidToIndex.erase(slot.uuid); //remove uuid cache
            slot.resource.reset(); // Free the underlying resource (calls the dtor of resource class)
            slot.uuid = Utils::UUID::Invalid();
            ++slot.generation; // Invalidates old handles
            freeSlots.push_back(handle.index);
        }

        Core::ResourceHandle<T> getHandleFromUUID(Utils::UUID uuid) {
            auto it = uuidToIndex.find(uuid);
            if (it == uuidToIndex.end()) {
                return {};
            }

            uint32_t index = it->second;
            auto& slot = slots[index];

            return {index, slot.generation, uuid};
        }

    private:
        //Store the typed pools as ptrs
        //std::unordered_map<Utils::UUID, std::unique_ptr<T>> resources;

        std::vector<Core::ResourceSlot<T>> slots;
        std::vector<uint32_t> freeSlots;
        std::unordered_map<Utils::UUID, uint32_t> uuidToIndex; // Conv map to convert ebtween the slotID and UUID



    };

}


namespace Dexium::Core {

    class ResourceManager {
    public:

        // Access type-specific resource pool or create one
        template<typename T>
        Private::Interfaces::ResourcePool<T>& getPool() {
            auto type = std::type_index(typeid(T));

            auto it = pools.find(type);
            if (it == pools.end()) {
                // No pool of type T exists, create it
                auto pool = std::make_unique<Private::Interfaces::ResourcePool<T>>();
                // Store ptr
                auto* ptr = pool.get();
                //Move into master pool
                pools[type] = std::move(pool);
                return *ptr;
            }
            // Else, fetch stored pool
            return *static_cast<Private::Interfaces::ResourcePool<T>*>(it->second.get());
        }

        // Access a cached resource directly from its pool
        template <typename T>
        T* get(const ResourceHandle<T>& handle) {
            return getPool<T>().get(handle);
        }

        // Store a resource handle
        template <typename T>
        ResourceHandle<T> store(std::unique_ptr<T> resource, std::string label = "") {
            auto& pool = getPool<T>();

            auto handle = pool.add(std::move(resource));
            handle.label = std::move(label);

            return handle;
        }
    private:
        std::unordered_map<std::type_index, std::unique_ptr<Private::Interfaces::IPool>> pools; // master pool

    };
}

#endif //DEXIUM_RESOURCEPOOL_HPP