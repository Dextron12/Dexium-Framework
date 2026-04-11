//
// Created by ethan on 11/4/26.
//

#ifndef DEXIUM_REFERENCE_HPP
#define DEXIUM_REFERENCE_HPP


#include <core/ResourcePool.hpp>

// A Reference system that works over ResourceHandle and raw ptr's

namespace Dexium::Core {

    template<typename T>
    class Reference {
    public:
        Reference(ResourceHandle<T> handle) : m_handle(handle) {}

        T* get(ResourceManager* rm = nullptr) const {
            if (m_handle.isValid()) {
                // If m_handle is invalid, it will default to m_ptr
                // m_ptr is by defualt invalid, so if both are invlaid Ref is invalid.

                // Check if the ResourceMnaager ptr is valid
                if (rm != nullptr) {
                    // Fetch stored ptr from ResourceManager
                    return rm->get(m_handle);
                }

                // else, use default (m_ptr):
                return m_ptr;
            }
        }

        T& operator*() const{ return *get();}
        T* operator->() const { return get(); }

        bool isValid() const {
            return get() != nullptr;
        }

    private:
        ResourceHandle<T> m_handle{};
        T* m_ptr = nullptr;
    };
}

#endif //DEXIUM_REFERENCE_HPP