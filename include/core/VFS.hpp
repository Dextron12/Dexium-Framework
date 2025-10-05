//
// Created by Dextron12 on 6/10/2025.
// DEXIUM-FRAMEWORK (C) Dextron12, 2025

/*
 * The Dexium::VFs (Virtual File System) is a utitly class that provides
 * executableDir lookup(on initilisation) path and file validation using
 * resolve() to convert from relPath to absPath and exists() to determine
 * if a file(regardless of path) exists within the project
 *
 * the VFS will also be used to *mount* a dexpak file, laod files into memory and decrypt the pack when needed
 */

#ifndef DEXIUM_VFS_HPP
#define DEXIUM_VFS_HPP


#include <memory>
#include <string>
#include <filesystem>

namespace Dexium {


    class VFS {
    public:
        static void Init();

        static std::unique_ptr<std::filesystem::path> resolve(const std::string& relPath);

        static bool exits (const std::string& path);
        static bool exits(const std::filesystem::path& path);

        static std::filesystem::path getExecutablePath();

    private:
        static std::filesystem::path _execPath;
    };


}


#endif //DEXIUM_VFS_HPP