//
// Created by Dextron12 on 2/12/25.
//

#ifndef DEXIUM_VFS_HPP
#define DEXIUM_VFS_HPP

#include <filesystem>

namespace Dexium::Core {

    class VFS {
    public:
        static void init(); // Gets the execPath (deduces platform)

        // Joins execPath + relpath and validates. returns the abs path on success or filesystem::path::empty() on failure
        static std::filesystem::path resolve(std::filesystem::path relPath);

        //Validates relative paths, sue filesystem::exist for abs paths
        static bool exists(std::filesystem::path relPath);

        // Returns the execPath (as it is stored)
        static std::filesystem::path getExecutablePath();

        // Ovverrides the execPath(working dir). Should really only be used in debug builds. In rel there should be no need to change working dirs
        static void overwriteExecPath(std::filesystem::path execPath);

    private:
        static std::filesystem::path m_execPath;

        VFS() = default;

    };
}

#endif //DEXIUM_VFS_HPP