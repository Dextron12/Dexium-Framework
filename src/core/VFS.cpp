//
// Created by Dextron12 on 2/12/25.
//

#include <core/VFS.hpp>
#include <core/Error.hpp>

#include <iostream>

#ifdef __linux__
#include <unistd.h>
#include <linux/limits.h>
#endif

namespace Dexium::Core {

    // Static definition
    std::filesystem::path VFS::m_execPath;

    void VFS::init() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        m_execPath = std::filesystem::path(buffer).parent_path();
#endif
#ifdef __linux__
        char buf[PATH_MAX] = {0};
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
        if (len == -1) {
            std::cout << "readlink error" << std::endl;
        }
        buf[len] = '\0'; // null-termination
        m_execPath = std::filesystem::path(buf).parent_path();
#endif
    }


    /*std::filesystem::path VFS::resolve(std::filesystem::path relPath) {
        if (m_execPath.empty()) {
            std::cout << "VFS execPath si empty" << std::endl;
            return std::filesystem::path();
        }

        std::filesystem::path abs = (m_execPath / relPath).lexically_normal();

        if (std::filesystem::exists(abs)) {
            return abs;
        }

        // WARN: failed path validation
        std::cout << "Could not resolve rel path into abs path" << std::endl;
        return std::filesystem::path();
    }*/

    /* THIS FUNCTION IS ESSENTIALLY A POLICY-ENFORCING ASSET RESOLVER
     * It makes these assumptions:
     * 1 - The asset/path in question exists(no future/pending writes of a new asset)
     * 2- Hard logging here and throughout the engine is expected
     * 3 - Invariants are forced early
     *
     * In the future, this class can be extented to sue a bitwise flag system that allows the engine & end-user to opt out off validating the files/paths
     */
    std::filesystem::path VFS::resolve(std::filesystem::path path) {
        if (m_execPath.empty()) {
            TraceLog(LogLevel::ERROR, "[VFS]: Engine root directoy is NOT defined.\nDid you forget to call VFS::init()?");
            // We could return an empty apth(fail) here, but what if an end-user uses abs paths without initialisation? Hence, why we dont early exit and providing an error, should be suffiocient enough
            // In any case, the engine is designed to validate paths when critical. So if they are using a rel apth without initilisation, this likely wont be the only error the end-user sees
        }

        // Dexium treats paths starting with '/' as either:
        // 1) true OS absolute paths (if they exist)
        // 2) engine-root-relative paths (virtual root)
        if (path.is_absolute()) {
            // Check if the path exists
            if (std::filesystem::exists(path)) return path; // Path is ABS and validated
            else {
                // Path identifies as ABS, but cannot be validated. Likely a rel from engine path onwards
                // Treat the engine dir as the virtual root + rel path
                if (path.has_root_directory() && !path.has_root_name()) {
                    return (m_execPath / path.relative_path()).lexically_normal();
                }
            }
            TraceLog(LogLevel::ERROR, "[VFS]: Identified '{}' as an abs path, but cannot validate it", path.string());
            return {};
        }

        // Otherwise treat as a relative path from the virtual root
        auto abs = (m_execPath / path).lexically_normal();
        if (std::filesystem::exists(abs)) return abs;
        else {
            TraceLog(LogLevel::ERROR, "[VFS]: Identified '{}' has a relative path, attempted to resolve from virtual_path into: {}, but, the abs path is not valid", path.string(), abs.string());
        }

        return {}; // Failed tor eoslve for some reason or the other
    }

    bool VFS::exists(std::filesystem::path relPath) {
        auto path = resolve(relPath);
        if (!path.empty()) { return true; }
        return false;
    }

    std::filesystem::path VFS::getExecutablePath() {
        return m_execPath;
    }

    void VFS::overwriteExecPath(std::filesystem::path execPath) {
        if (std::filesystem::exists(execPath) && std::filesystem::is_directory(execPath)) {
            m_execPath = execPath;
        } else {
            std::cout << "cannot change execPath to: " << execPath << " it is invalid" << std::endl;
        }
    }





}
