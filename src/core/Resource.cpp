//
// Created by Dextron12 on 19/11/2025.
//

#include <core/Resource.hpp>
#include <core/VFS.hpp>

Dexium::Core::FileEntry::FileEntry(const std::string &data) {
    auto isFile = VFS::resolve(data);
    if (isFile) {
        isPathValid = true;
        lastModified = std::filesystem::last_write_time(*isFile);
        data = isFile->string();
    } else {
        isPathValid = false;
        // Dont touch data as its likely raw streamed data for an Asset class
    }
}

bool Dexium::Core::FileEntry::shouldReload() const {
    if (isPathValid) {
        auto currentTime = std::filesystem::last_write_time(data);
        if (currentTime != lastModified) {
            // File updated, return true
            return true;
        }
    }

    return false;
}
